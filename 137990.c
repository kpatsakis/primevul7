static uint16_t nvme_verify(NvmeCtrl *n, NvmeRequest *req)
{
    NvmeRwCmd *rw = (NvmeRwCmd *)&req->cmd;
    NvmeNamespace *ns = req->ns;
    BlockBackend *blk = ns->blkconf.blk;
    uint64_t slba = le64_to_cpu(rw->slba);
    uint32_t nlb = le16_to_cpu(rw->nlb) + 1;
    size_t len = nvme_l2b(ns, nlb);
    int64_t offset = nvme_l2b(ns, slba);
    uint8_t prinfo = NVME_RW_PRINFO(le16_to_cpu(rw->control));
    uint32_t reftag = le32_to_cpu(rw->reftag);
    NvmeBounceContext *ctx = NULL;
    uint16_t status;

    trace_pci_nvme_verify(nvme_cid(req), nvme_nsid(ns), slba, nlb);

    if (NVME_ID_NS_DPS_TYPE(ns->id_ns.dps)) {
        status = nvme_check_prinfo(ns, prinfo, slba, reftag);
        if (status) {
            return status;
        }

        if (prinfo & NVME_PRINFO_PRACT) {
            return NVME_INVALID_PROT_INFO | NVME_DNR;
        }
    }

    if (len > n->page_size << n->params.vsl) {
        return NVME_INVALID_FIELD | NVME_DNR;
    }

    status = nvme_check_bounds(ns, slba, nlb);
    if (status) {
        return status;
    }

    if (NVME_ERR_REC_DULBE(ns->features.err_rec)) {
        status = nvme_check_dulbe(ns, slba, nlb);
        if (status) {
            return status;
        }
    }

    ctx = g_new0(NvmeBounceContext, 1);
    ctx->req = req;

    ctx->data.bounce = g_malloc(len);

    qemu_iovec_init(&ctx->data.iov, 1);
    qemu_iovec_add(&ctx->data.iov, ctx->data.bounce, len);

    block_acct_start(blk_get_stats(blk), &req->acct, ctx->data.iov.size,
                     BLOCK_ACCT_READ);

    req->aiocb = blk_aio_preadv(ns->blkconf.blk, offset, &ctx->data.iov, 0,
                                nvme_verify_mdata_in_cb, ctx);
    return NVME_NO_COMPLETE;
}