static void nvme_flush_ns_cb(void *opaque, int ret)
{
    NvmeFlushAIOCB *iocb = opaque;
    NvmeNamespace *ns = iocb->ns;

    if (ret < 0) {
        iocb->ret = ret;
        goto out;
    } else if (iocb->ret < 0) {
        goto out;
    }

    if (ns) {
        trace_pci_nvme_flush_ns(iocb->nsid);

        iocb->ns = NULL;
        iocb->aiocb = blk_aio_flush(ns->blkconf.blk, nvme_flush_ns_cb, iocb);
        return;
    }

out:
    iocb->aiocb = NULL;
    qemu_bh_schedule(iocb->bh);
}