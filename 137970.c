static void nvme_dsm_bh(void *opaque)
{
    NvmeDSMAIOCB *iocb = opaque;

    iocb->common.cb(iocb->common.opaque, iocb->ret);

    qemu_bh_delete(iocb->bh);
    iocb->bh = NULL;
    qemu_aio_unref(iocb);
}