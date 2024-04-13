static void nvme_zone_reset_cancel(BlockAIOCB *aiocb)
{
    NvmeZoneResetAIOCB *iocb = container_of(aiocb, NvmeZoneResetAIOCB, common);
    NvmeRequest *req = iocb->req;
    NvmeNamespace *ns = req->ns;

    iocb->idx = ns->num_zones;

    iocb->ret = -ECANCELED;

    if (iocb->aiocb) {
        blk_aio_cancel_async(iocb->aiocb);
        iocb->aiocb = NULL;
    }
}