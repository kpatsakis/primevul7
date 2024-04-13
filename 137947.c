void nvme_rw_complete_cb(void *opaque, int ret)
{
    NvmeRequest *req = opaque;
    NvmeNamespace *ns = req->ns;
    BlockBackend *blk = ns->blkconf.blk;
    BlockAcctCookie *acct = &req->acct;
    BlockAcctStats *stats = blk_get_stats(blk);

    trace_pci_nvme_rw_complete_cb(nvme_cid(req), blk_name(blk));

    if (ret) {
        block_acct_failed(stats, acct);
        nvme_aio_err(req, ret);
    } else {
        block_acct_done(stats, acct);
    }

    if (ns->params.zoned && nvme_is_write(req)) {
        nvme_finalize_zoned_write(ns, req);
    }

    nvme_enqueue_req_completion(nvme_cq(req), req);
}