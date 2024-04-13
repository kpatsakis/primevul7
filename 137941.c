static void nvme_copy_out_cb(void *opaque, int ret)
{
    NvmeCopyAIOCB *iocb = opaque;
    NvmeRequest *req = iocb->req;
    NvmeNamespace *ns = req->ns;
    NvmeCopySourceRange *range;
    uint32_t nlb;
    size_t mlen;
    uint8_t *mbounce;

    if (ret < 0) {
        iocb->ret = ret;
        goto out;
    } else if (iocb->ret < 0) {
        goto out;
    }

    if (!ns->lbaf.ms) {
        nvme_copy_out_completed_cb(iocb, 0);
        return;
    }

    range = &iocb->ranges[iocb->idx];
    nlb = le32_to_cpu(range->nlb) + 1;

    mlen = nvme_m2b(ns, nlb);
    mbounce = iocb->bounce + nvme_l2b(ns, nlb);

    qemu_iovec_reset(&iocb->iov);
    qemu_iovec_add(&iocb->iov, mbounce, mlen);

    iocb->aiocb = blk_aio_pwritev(ns->blkconf.blk, nvme_moff(ns, iocb->slba),
                                  &iocb->iov, 0, nvme_copy_out_completed_cb,
                                  iocb);

    return;

out:
    nvme_copy_cb(iocb, ret);
}