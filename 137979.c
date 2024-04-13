static uint16_t nvme_compare(NvmeCtrl *n, NvmeRequest *req)
{
    NvmeRwCmd *rw = (NvmeRwCmd *)&req->cmd;
    NvmeNamespace *ns = req->ns;
    BlockBackend *blk = ns->blkconf.blk;
    uint64_t slba = le64_to_cpu(rw->slba);
    uint32_t nlb = le16_to_cpu(rw->nlb) + 1;
    uint8_t prinfo = NVME_RW_PRINFO(le16_to_cpu(rw->control));
    size_t data_len = nvme_l2b(ns, nlb);
    size_t len = data_len;
    int64_t offset = nvme_l2b(ns, slba);
    struct nvme_compare_ctx *ctx = NULL;
    uint16_t status;

    trace_pci_nvme_compare(nvme_cid(req), nvme_nsid(ns), slba, nlb);

    if (NVME_ID_NS_DPS_TYPE(ns->id_ns.dps) && (prinfo & NVME_PRINFO_PRACT)) {
        return NVME_INVALID_PROT_INFO | NVME_DNR;
    }

    if (nvme_ns_ext(ns)) {
        len += nvme_m2b(ns, nlb);
    }

    status = nvme_check_mdts(n, len);
    if (status) {
        return status;
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

    status = nvme_map_dptr(n, &req->sg, len, &req->cmd);
    if (status) {
        return status;
    }

    ctx = g_new(struct nvme_compare_ctx, 1);
    ctx->data.bounce = g_malloc(data_len);

    req->opaque = ctx;

    qemu_iovec_init(&ctx->data.iov, 1);
    qemu_iovec_add(&ctx->data.iov, ctx->data.bounce, data_len);

    block_acct_start(blk_get_stats(blk), &req->acct, data_len,
                     BLOCK_ACCT_READ);
    req->aiocb = blk_aio_preadv(blk, offset, &ctx->data.iov, 0,
                                nvme_compare_data_cb, req);

    return NVME_NO_COMPLETE;
}