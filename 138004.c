static void nvme_verify_cb(void *opaque, int ret)
{
    NvmeBounceContext *ctx = opaque;
    NvmeRequest *req = ctx->req;
    NvmeNamespace *ns = req->ns;
    BlockBackend *blk = ns->blkconf.blk;
    BlockAcctCookie *acct = &req->acct;
    BlockAcctStats *stats = blk_get_stats(blk);
    NvmeRwCmd *rw = (NvmeRwCmd *)&req->cmd;
    uint64_t slba = le64_to_cpu(rw->slba);
    uint8_t prinfo = NVME_RW_PRINFO(le16_to_cpu(rw->control));
    uint16_t apptag = le16_to_cpu(rw->apptag);
    uint16_t appmask = le16_to_cpu(rw->appmask);
    uint32_t reftag = le32_to_cpu(rw->reftag);
    uint16_t status;

    trace_pci_nvme_verify_cb(nvme_cid(req), prinfo, apptag, appmask, reftag);

    if (ret) {
        block_acct_failed(stats, acct);
        nvme_aio_err(req, ret);
        goto out;
    }

    block_acct_done(stats, acct);

    if (NVME_ID_NS_DPS_TYPE(ns->id_ns.dps)) {
        status = nvme_dif_mangle_mdata(ns, ctx->mdata.bounce,
                                       ctx->mdata.iov.size, slba);
        if (status) {
            req->status = status;
            goto out;
        }

        req->status = nvme_dif_check(ns, ctx->data.bounce, ctx->data.iov.size,
                                     ctx->mdata.bounce, ctx->mdata.iov.size,
                                     prinfo, slba, apptag, appmask, &reftag);
    }

out:
    qemu_iovec_destroy(&ctx->data.iov);
    g_free(ctx->data.bounce);

    qemu_iovec_destroy(&ctx->mdata.iov);
    g_free(ctx->mdata.bounce);

    g_free(ctx);

    nvme_enqueue_req_completion(nvme_cq(req), req);
}