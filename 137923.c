static void nvme_copy_cb(void *opaque, int ret)
{
    NvmeCopyAIOCB *iocb = opaque;
    NvmeRequest *req = iocb->req;
    NvmeNamespace *ns = req->ns;
    NvmeCopySourceRange *range;
    uint64_t slba;
    uint32_t nlb;
    size_t len;
    uint16_t status;

    if (ret < 0) {
        iocb->ret = ret;
        goto done;
    } else if (iocb->ret < 0) {
        goto done;
    }

    if (iocb->idx == iocb->nr) {
        goto done;
    }

    range = &iocb->ranges[iocb->idx];
    slba = le64_to_cpu(range->slba);
    nlb = le32_to_cpu(range->nlb) + 1;
    len = nvme_l2b(ns, nlb);

    trace_pci_nvme_copy_source_range(slba, nlb);

    if (nlb > le16_to_cpu(ns->id_ns.mssrl)) {
        status = NVME_CMD_SIZE_LIMIT | NVME_DNR;
        goto invalid;
    }

    status = nvme_check_bounds(ns, slba, nlb);
    if (status) {
        goto invalid;
    }

    if (NVME_ERR_REC_DULBE(ns->features.err_rec)) {
        status = nvme_check_dulbe(ns, slba, nlb);
        if (status) {
            goto invalid;
        }
    }

    if (ns->params.zoned) {
        status = nvme_check_zone_read(ns, slba, nlb);
        if (status) {
            goto invalid;
        }
    }

    qemu_iovec_reset(&iocb->iov);
    qemu_iovec_add(&iocb->iov, iocb->bounce, len);

    iocb->aiocb = blk_aio_preadv(ns->blkconf.blk, nvme_l2b(ns, slba),
                                 &iocb->iov, 0, nvme_copy_in_cb, iocb);
    return;

invalid:
    req->status = status;
done:
    iocb->aiocb = NULL;
    if (iocb->bh) {
        qemu_bh_schedule(iocb->bh);
    }
}