static void nvme_compare_mdata_cb(void *opaque, int ret)
{
    NvmeRequest *req = opaque;
    NvmeNamespace *ns = req->ns;
    NvmeCtrl *n = nvme_ctrl(req);
    NvmeRwCmd *rw = (NvmeRwCmd *)&req->cmd;
    uint8_t prinfo = NVME_RW_PRINFO(le16_to_cpu(rw->control));
    uint16_t apptag = le16_to_cpu(rw->apptag);
    uint16_t appmask = le16_to_cpu(rw->appmask);
    uint32_t reftag = le32_to_cpu(rw->reftag);
    struct nvme_compare_ctx *ctx = req->opaque;
    g_autofree uint8_t *buf = NULL;
    BlockBackend *blk = ns->blkconf.blk;
    BlockAcctCookie *acct = &req->acct;
    BlockAcctStats *stats = blk_get_stats(blk);
    uint16_t status = NVME_SUCCESS;

    trace_pci_nvme_compare_mdata_cb(nvme_cid(req));

    if (ret) {
        block_acct_failed(stats, acct);
        nvme_aio_err(req, ret);
        goto out;
    }

    buf = g_malloc(ctx->mdata.iov.size);

    status = nvme_bounce_mdata(n, buf, ctx->mdata.iov.size,
                               NVME_TX_DIRECTION_TO_DEVICE, req);
    if (status) {
        req->status = status;
        goto out;
    }

    if (NVME_ID_NS_DPS_TYPE(ns->id_ns.dps)) {
        uint64_t slba = le64_to_cpu(rw->slba);
        uint8_t *bufp;
        uint8_t *mbufp = ctx->mdata.bounce;
        uint8_t *end = mbufp + ctx->mdata.iov.size;
        int16_t pil = 0;

        status = nvme_dif_check(ns, ctx->data.bounce, ctx->data.iov.size,
                                ctx->mdata.bounce, ctx->mdata.iov.size, prinfo,
                                slba, apptag, appmask, &reftag);
        if (status) {
            req->status = status;
            goto out;
        }

        /*
         * When formatted with protection information, do not compare the DIF
         * tuple.
         */
        if (!(ns->id_ns.dps & NVME_ID_NS_DPS_FIRST_EIGHT)) {
            pil = ns->lbaf.ms - sizeof(NvmeDifTuple);
        }

        for (bufp = buf; mbufp < end; bufp += ns->lbaf.ms, mbufp += ns->lbaf.ms) {
            if (memcmp(bufp + pil, mbufp + pil, ns->lbaf.ms - pil)) {
                req->status = NVME_CMP_FAILURE;
                goto out;
            }
        }

        goto out;
    }

    if (memcmp(buf, ctx->mdata.bounce, ctx->mdata.iov.size)) {
        req->status = NVME_CMP_FAILURE;
        goto out;
    }

    block_acct_done(stats, acct);

out:
    qemu_iovec_destroy(&ctx->data.iov);
    g_free(ctx->data.bounce);

    qemu_iovec_destroy(&ctx->mdata.iov);
    g_free(ctx->mdata.bounce);

    g_free(ctx);

    nvme_enqueue_req_completion(nvme_cq(req), req);
}