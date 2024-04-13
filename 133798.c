do_sync_target_write(MirrorBlockJob *job, MirrorMethod method,
                     uint64_t offset, uint64_t bytes,
                     QEMUIOVector *qiov, int flags)
{
    int ret;
    size_t qiov_offset = 0;
    int64_t bitmap_offset, bitmap_end;

    if (!QEMU_IS_ALIGNED(offset, job->granularity) &&
        bdrv_dirty_bitmap_get(job->dirty_bitmap, offset))
    {
            /*
             * Dirty unaligned padding: ignore it.
             *
             * Reasoning:
             * 1. If we copy it, we can't reset corresponding bit in
             *    dirty_bitmap as there may be some "dirty" bytes still not
             *    copied.
             * 2. It's already dirty, so skipping it we don't diverge mirror
             *    progress.
             *
             * Note, that because of this, guest write may have no contribution
             * into mirror converge, but that's not bad, as we have background
             * process of mirroring. If under some bad circumstances (high guest
             * IO load) background process starve, we will not converge anyway,
             * even if each write will contribute, as guest is not guaranteed to
             * rewrite the whole disk.
             */
            qiov_offset = QEMU_ALIGN_UP(offset, job->granularity) - offset;
            if (bytes <= qiov_offset) {
                /* nothing to do after shrink */
                return;
            }
            offset += qiov_offset;
            bytes -= qiov_offset;
    }

    if (!QEMU_IS_ALIGNED(offset + bytes, job->granularity) &&
        bdrv_dirty_bitmap_get(job->dirty_bitmap, offset + bytes - 1))
    {
        uint64_t tail = (offset + bytes) % job->granularity;

        if (bytes <= tail) {
            /* nothing to do after shrink */
            return;
        }
        bytes -= tail;
    }

    /*
     * Tails are either clean or shrunk, so for bitmap resetting
     * we safely align the range down.
     */
    bitmap_offset = QEMU_ALIGN_UP(offset, job->granularity);
    bitmap_end = QEMU_ALIGN_DOWN(offset + bytes, job->granularity);
    if (bitmap_offset < bitmap_end) {
        bdrv_reset_dirty_bitmap(job->dirty_bitmap, bitmap_offset,
                                bitmap_end - bitmap_offset);
    }

    job_progress_increase_remaining(&job->common.job, bytes);

    switch (method) {
    case MIRROR_METHOD_COPY:
        ret = blk_co_pwritev_part(job->target, offset, bytes,
                                  qiov, qiov_offset, flags);
        break;

    case MIRROR_METHOD_ZERO:
        assert(!qiov);
        ret = blk_co_pwrite_zeroes(job->target, offset, bytes, flags);
        break;

    case MIRROR_METHOD_DISCARD:
        assert(!qiov);
        ret = blk_co_pdiscard(job->target, offset, bytes);
        break;

    default:
        abort();
    }

    if (ret >= 0) {
        job_progress_update(&job->common.job, bytes);
    } else {
        BlockErrorAction action;

        /*
         * We failed, so we should mark dirty the whole area, aligned up.
         * Note that we don't care about shrunk tails if any: they were dirty
         * at function start, and they must be still dirty, as we've locked
         * the region for in-flight op.
         */
        bitmap_offset = QEMU_ALIGN_DOWN(offset, job->granularity);
        bitmap_end = QEMU_ALIGN_UP(offset + bytes, job->granularity);
        bdrv_set_dirty_bitmap(job->dirty_bitmap, bitmap_offset,
                              bitmap_end - bitmap_offset);
        job->actively_synced = false;

        action = mirror_error_action(job, false, -ret);
        if (action == BLOCK_ERROR_ACTION_REPORT) {
            if (!job->ret) {
                job->ret = ret;
            }
        }
    }
}