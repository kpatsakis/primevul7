static void nvme_copy_bh(void *opaque)
{
    NvmeCopyAIOCB *iocb = opaque;
    NvmeRequest *req = iocb->req;
    NvmeNamespace *ns = req->ns;
    BlockAcctStats *stats = blk_get_stats(ns->blkconf.blk);

    if (iocb->idx != iocb->nr) {
        req->cqe.result = cpu_to_le32(iocb->idx);
    }

    qemu_iovec_destroy(&iocb->iov);
    g_free(iocb->bounce);

    qemu_bh_delete(iocb->bh);
    iocb->bh = NULL;

    if (iocb->ret < 0) {
        block_acct_failed(stats, &iocb->acct.read);
        block_acct_failed(stats, &iocb->acct.write);
    } else {
        block_acct_done(stats, &iocb->acct.read);
        block_acct_done(stats, &iocb->acct.write);
    }

    iocb->common.cb(iocb->common.opaque, iocb->ret);
    qemu_aio_unref(iocb);
}