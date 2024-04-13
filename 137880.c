static void nvme_zone_reset_epilogue_cb(void *opaque, int ret)
{
    NvmeZoneResetAIOCB *iocb = opaque;
    NvmeRequest *req = iocb->req;
    NvmeNamespace *ns = req->ns;
    int64_t moff;
    int count;

    if (ret < 0) {
        nvme_zone_reset_cb(iocb, ret);
        return;
    }

    if (!ns->lbaf.ms) {
        nvme_zone_reset_cb(iocb, 0);
        return;
    }

    moff = nvme_moff(ns, iocb->zone->d.zslba);
    count = nvme_m2b(ns, ns->zone_size);

    iocb->aiocb = blk_aio_pwrite_zeroes(ns->blkconf.blk, moff, count,
                                        BDRV_REQ_MAY_UNMAP,
                                        nvme_zone_reset_cb, iocb);
    return;
}