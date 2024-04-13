uint16_t nvme_bounce_data(NvmeCtrl *n, uint8_t *ptr, uint32_t len,
                          NvmeTxDirection dir, NvmeRequest *req)
{
    NvmeNamespace *ns = req->ns;
    NvmeRwCmd *rw = (NvmeRwCmd *)&req->cmd;
    bool pi = !!NVME_ID_NS_DPS_TYPE(ns->id_ns.dps);
    bool pract = !!(le16_to_cpu(rw->control) & NVME_RW_PRINFO_PRACT);

    if (nvme_ns_ext(ns) && !(pi && pract && ns->lbaf.ms == 8)) {
        return nvme_tx_interleaved(n, &req->sg, ptr, len, ns->lbasz,
                                   ns->lbaf.ms, 0, dir);
    }

    return nvme_tx(n, &req->sg, ptr, len, dir);
}