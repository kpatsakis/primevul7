static void nvme_format_ns_cb(void *opaque, int ret)
{
    NvmeFormatAIOCB *iocb = opaque;
    NvmeRequest *req = iocb->req;
    NvmeNamespace *ns = iocb->ns;
    int bytes;

    if (ret < 0) {
        iocb->ret = ret;
        goto done;
    }

    assert(ns);

    if (iocb->offset < ns->size) {
        bytes = MIN(BDRV_REQUEST_MAX_BYTES, ns->size - iocb->offset);

        iocb->aiocb = blk_aio_pwrite_zeroes(ns->blkconf.blk, iocb->offset,
                                            bytes, BDRV_REQ_MAY_UNMAP,
                                            nvme_format_ns_cb, iocb);

        iocb->offset += bytes;
        return;
    }

    nvme_format_set(ns, &req->cmd);
    ns->status = 0x0;
    iocb->ns = NULL;
    iocb->offset = 0;

done:
    iocb->aiocb = NULL;
    qemu_bh_schedule(iocb->bh);
}