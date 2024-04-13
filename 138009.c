uint16_t nvme_bounce_mdata(NvmeCtrl *n, uint8_t *ptr, uint32_t len,
                           NvmeTxDirection dir, NvmeRequest *req)
{
    NvmeNamespace *ns = req->ns;
    uint16_t status;

    if (nvme_ns_ext(ns)) {
        return nvme_tx_interleaved(n, &req->sg, ptr, len, ns->lbaf.ms,
                                   ns->lbasz, ns->lbasz, dir);
    }

    nvme_sg_unmap(&req->sg);

    status = nvme_map_mptr(n, &req->sg, len, &req->cmd);
    if (status) {
        return status;
    }

    return nvme_tx(n, &req->sg, ptr, len, dir);
}