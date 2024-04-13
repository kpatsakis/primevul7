static int coroutine_fn iscsi_co_readv(BlockDriverState *bs,
                                       int64_t sector_num, int nb_sectors,
                                       QEMUIOVector *iov)
{
    IscsiLun *iscsilun = bs->opaque;
    struct IscsiTask iTask;
    uint64_t lba;
    uint32_t num_sectors;
    int r = 0;

    if (!is_sector_request_lun_aligned(sector_num, nb_sectors, iscsilun)) {
        return -EINVAL;
    }

    if (bs->bl.max_transfer) {
        assert(nb_sectors << BDRV_SECTOR_BITS <= bs->bl.max_transfer);
    }

    /* if cache.direct is off and we have a valid entry in our allocation map
     * we can skip checking the block status and directly return zeroes if
     * the request falls within an unallocated area */
    if (iscsi_allocmap_is_valid(iscsilun, sector_num * BDRV_SECTOR_SIZE,
                                nb_sectors * BDRV_SECTOR_SIZE) &&
        !iscsi_allocmap_is_allocated(iscsilun, sector_num * BDRV_SECTOR_SIZE,
                                     nb_sectors * BDRV_SECTOR_SIZE)) {
            qemu_iovec_memset(iov, 0, 0x00, iov->size);
            return 0;
    }

    if (nb_sectors >= ISCSI_CHECKALLOC_THRES &&
        !iscsi_allocmap_is_valid(iscsilun, sector_num * BDRV_SECTOR_SIZE,
                                 nb_sectors * BDRV_SECTOR_SIZE) &&
        !iscsi_allocmap_is_allocated(iscsilun, sector_num * BDRV_SECTOR_SIZE,
                                     nb_sectors * BDRV_SECTOR_SIZE)) {
        int64_t pnum;
        /* check the block status from the beginning of the cluster
         * containing the start sector */
        int64_t head;
        int ret;

        assert(iscsilun->cluster_size);
        head = (sector_num * BDRV_SECTOR_SIZE) % iscsilun->cluster_size;
        ret = iscsi_co_block_status(bs, true,
                                    sector_num * BDRV_SECTOR_SIZE - head,
                                    BDRV_REQUEST_MAX_BYTES, &pnum, NULL, NULL);
        if (ret < 0) {
            return ret;
        }
        /* if the whole request falls into an unallocated area we can avoid
         * reading and directly return zeroes instead */
        if (ret & BDRV_BLOCK_ZERO &&
            pnum >= nb_sectors * BDRV_SECTOR_SIZE + head) {
            qemu_iovec_memset(iov, 0, 0x00, iov->size);
            return 0;
        }
    }

    lba = sector_qemu2lun(sector_num, iscsilun);
    num_sectors = sector_qemu2lun(nb_sectors, iscsilun);

    iscsi_co_init_iscsitask(iscsilun, &iTask);
    qemu_mutex_lock(&iscsilun->mutex);
retry:
    if (iscsilun->use_16_for_rw) {
#if LIBISCSI_API_VERSION >= (20160603)
        iTask.task = iscsi_read16_iov_task(iscsilun->iscsi, iscsilun->lun, lba,
                                           num_sectors * iscsilun->block_size,
                                           iscsilun->block_size, 0, 0, 0, 0, 0,
                                           iscsi_co_generic_cb, &iTask,
                                           (struct scsi_iovec *)iov->iov, iov->niov);
    } else {
        iTask.task = iscsi_read10_iov_task(iscsilun->iscsi, iscsilun->lun, lba,
                                           num_sectors * iscsilun->block_size,
                                           iscsilun->block_size,
                                           0, 0, 0, 0, 0,
                                           iscsi_co_generic_cb, &iTask,
                                           (struct scsi_iovec *)iov->iov, iov->niov);
    }
#else
        iTask.task = iscsi_read16_task(iscsilun->iscsi, iscsilun->lun, lba,
                                       num_sectors * iscsilun->block_size,
                                       iscsilun->block_size, 0, 0, 0, 0, 0,
                                       iscsi_co_generic_cb, &iTask);
    } else {