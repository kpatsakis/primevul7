static void nvme_dsm_md_cb(void *opaque, int ret)
{
    NvmeDSMAIOCB *iocb = opaque;
    NvmeRequest *req = iocb->req;
    NvmeNamespace *ns = req->ns;
    NvmeDsmRange *range;
    uint64_t slba;
    uint32_t nlb;

    if (ret < 0) {
        iocb->ret = ret;
        goto done;
    }

    if (!ns->lbaf.ms) {
        nvme_dsm_cb(iocb, 0);
        return;
    }

    range = &iocb->range[iocb->idx - 1];
    slba = le64_to_cpu(range->slba);
    nlb = le32_to_cpu(range->nlb);

    /*
     * Check that all block were discarded (zeroed); otherwise we do not zero
     * the metadata.
     */

    ret = nvme_block_status_all(ns, slba, nlb, BDRV_BLOCK_ZERO);
    if (ret) {
        if (ret < 0) {
            iocb->ret = ret;
            goto done;
        }

        nvme_dsm_cb(iocb, 0);
    }

    iocb->aiocb = blk_aio_pwrite_zeroes(ns->blkconf.blk, nvme_moff(ns, slba),
                                        nvme_m2b(ns, nlb), BDRV_REQ_MAY_UNMAP,
                                        nvme_dsm_cb, iocb);
    return;

done:
    iocb->aiocb = NULL;
    qemu_bh_schedule(iocb->bh);
}