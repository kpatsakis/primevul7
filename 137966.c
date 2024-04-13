static void nvme_verify_mdata_in_cb(void *opaque, int ret)
{
    NvmeBounceContext *ctx = opaque;
    NvmeRequest *req = ctx->req;
    NvmeNamespace *ns = req->ns;
    NvmeRwCmd *rw = (NvmeRwCmd *)&req->cmd;
    uint64_t slba = le64_to_cpu(rw->slba);
    uint32_t nlb = le16_to_cpu(rw->nlb) + 1;
    size_t mlen = nvme_m2b(ns, nlb);
    uint64_t offset = nvme_moff(ns, slba);
    BlockBackend *blk = ns->blkconf.blk;

    trace_pci_nvme_verify_mdata_in_cb(nvme_cid(req), blk_name(blk));

    if (ret) {
        goto out;
    }

    ctx->mdata.bounce = g_malloc(mlen);

    qemu_iovec_reset(&ctx->mdata.iov);
    qemu_iovec_add(&ctx->mdata.iov, ctx->mdata.bounce, mlen);

    req->aiocb = blk_aio_preadv(blk, offset, &ctx->mdata.iov, 0,
                                nvme_verify_cb, ctx);
    return;

out:
    nvme_verify_cb(ctx, ret);
}