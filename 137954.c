static uint16_t nvme_format(NvmeCtrl *n, NvmeRequest *req)
{
    NvmeFormatAIOCB *iocb;
    uint32_t nsid = le32_to_cpu(req->cmd.nsid);
    uint16_t status;

    iocb = qemu_aio_get(&nvme_format_aiocb_info, NULL, nvme_misc_cb, req);

    iocb->req = req;
    iocb->bh = qemu_bh_new(nvme_format_bh, iocb);
    iocb->ret = 0;
    iocb->ns = NULL;
    iocb->nsid = 0;
    iocb->broadcast = (nsid == NVME_NSID_BROADCAST);
    iocb->offset = 0;

    if (!iocb->broadcast) {
        if (!nvme_nsid_valid(n, nsid)) {
            status = NVME_INVALID_NSID | NVME_DNR;
            goto out;
        }

        iocb->ns = nvme_ns(n, nsid);
        if (!iocb->ns) {
            status = NVME_INVALID_FIELD | NVME_DNR;
            goto out;
        }
    }

    req->aiocb = &iocb->common;
    qemu_bh_schedule(iocb->bh);

    return NVME_NO_COMPLETE;

out:
    qemu_bh_delete(iocb->bh);
    iocb->bh = NULL;
    qemu_aio_unref(iocb);
    return status;
}