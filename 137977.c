static void nvme_compare_data_cb(void *opaque, int ret)
{
    NvmeRequest *req = opaque;
    NvmeCtrl *n = nvme_ctrl(req);
    NvmeNamespace *ns = req->ns;
    BlockBackend *blk = ns->blkconf.blk;
    BlockAcctCookie *acct = &req->acct;
    BlockAcctStats *stats = blk_get_stats(blk);

    struct nvme_compare_ctx *ctx = req->opaque;
    g_autofree uint8_t *buf = NULL;
    uint16_t status;

    trace_pci_nvme_compare_data_cb(nvme_cid(req));

    if (ret) {
        block_acct_failed(stats, acct);
        nvme_aio_err(req, ret);
        goto out;
    }

    buf = g_malloc(ctx->data.iov.size);

    status = nvme_bounce_data(n, buf, ctx->data.iov.size,
                              NVME_TX_DIRECTION_TO_DEVICE, req);
    if (status) {
        req->status = status;
        goto out;
    }

    if (memcmp(buf, ctx->data.bounce, ctx->data.iov.size)) {
        req->status = NVME_CMP_FAILURE;
        goto out;
    }

    if (ns->lbaf.ms) {
        NvmeRwCmd *rw = (NvmeRwCmd *)&req->cmd;
        uint64_t slba = le64_to_cpu(rw->slba);
        uint32_t nlb = le16_to_cpu(rw->nlb) + 1;
        size_t mlen = nvme_m2b(ns, nlb);
        uint64_t offset = nvme_moff(ns, slba);

        ctx->mdata.bounce = g_malloc(mlen);

        qemu_iovec_init(&ctx->mdata.iov, 1);
        qemu_iovec_add(&ctx->mdata.iov, ctx->mdata.bounce, mlen);

        req->aiocb = blk_aio_preadv(blk, offset, &ctx->mdata.iov, 0,
                                    nvme_compare_mdata_cb, req);
        return;
    }

    block_acct_done(stats, acct);

out:
    qemu_iovec_destroy(&ctx->data.iov);
    g_free(ctx->data.bounce);
    g_free(ctx);

    nvme_enqueue_req_completion(nvme_cq(req), req);
}