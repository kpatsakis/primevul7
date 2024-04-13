static void nvme_copy_in_cb(void *opaque, int ret)
{
    NvmeCopyAIOCB *iocb = opaque;
    NvmeRequest *req = iocb->req;
    NvmeNamespace *ns = req->ns;
    NvmeCopySourceRange *range;
    uint64_t slba;
    uint32_t nlb;

    if (ret < 0) {
        iocb->ret = ret;
        goto out;
    } else if (iocb->ret < 0) {
        goto out;
    }

    if (!ns->lbaf.ms) {
        nvme_copy_in_completed_cb(iocb, 0);
        return;
    }

    range = &iocb->ranges[iocb->idx];
    slba = le64_to_cpu(range->slba);
    nlb = le32_to_cpu(range->nlb) + 1;

    qemu_iovec_reset(&iocb->iov);
    qemu_iovec_add(&iocb->iov, iocb->bounce + nvme_l2b(ns, nlb),
                   nvme_m2b(ns, nlb));

    iocb->aiocb = blk_aio_preadv(ns->blkconf.blk, nvme_moff(ns, slba),
                                 &iocb->iov, 0, nvme_copy_in_completed_cb,
                                 iocb);
    return;

out:
    nvme_copy_cb(iocb, iocb->ret);
}