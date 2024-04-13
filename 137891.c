static void nvme_format_bh(void *opaque)
{
    NvmeFormatAIOCB *iocb = opaque;
    NvmeRequest *req = iocb->req;
    NvmeCtrl *n = nvme_ctrl(req);
    uint32_t dw10 = le32_to_cpu(req->cmd.cdw10);
    uint8_t lbaf = dw10 & 0xf;
    uint8_t pi = (dw10 >> 5) & 0x7;
    uint16_t status;
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

    status = nvme_format_check(iocb->ns, lbaf, pi);
    if (status) {
        req->status = status;
        goto done;
    }

    iocb->ns->status = NVME_FORMAT_IN_PROGRESS;
    nvme_format_ns_cb(iocb, 0);
    return;

done:
    qemu_bh_delete(iocb->bh);
    iocb->bh = NULL;

    iocb->common.cb(iocb->common.opaque, iocb->ret);

    qemu_aio_unref(iocb);
}