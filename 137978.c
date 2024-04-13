static void nvme_zone_reset_bh(void *opaque)
{
    NvmeZoneResetAIOCB *iocb = opaque;

    iocb->common.cb(iocb->common.opaque, iocb->ret);

    qemu_bh_delete(iocb->bh);
    iocb->bh = NULL;
    qemu_aio_unref(iocb);
}