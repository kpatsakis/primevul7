static void nvme_flush_bh(void *opaque)
{
    NvmeFlushAIOCB *iocb = opaque;
    NvmeRequest *req = iocb->req;
    NvmeCtrl *n = nvme_ctrl(req);
    int i;

    if (iocb->ret < 0) {
        goto done;
    }

    if (iocb->broadcast) {
        for (i = iocb->nsid + 1; i <= NVME_MAX_NAMESPACES; i++) {
            iocb->ns = nvme_ns(n, i);
            if (iocb->ns) {
                iocb->nsid = i;
                break;
            }
        }
    }

    if (!iocb->ns) {
        goto done;
    }

    nvme_flush_ns_cb(iocb, 0);
    return;

done:
    qemu_bh_delete(iocb->bh);
    iocb->bh = NULL;

    iocb->common.cb(iocb->common.opaque, iocb->ret);

    qemu_aio_unref(iocb);

    return;
}