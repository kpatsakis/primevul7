static uint16_t nvme_map_data(NvmeCtrl *n, uint32_t nlb, NvmeRequest *req)
{
    NvmeNamespace *ns = req->ns;
    NvmeRwCmd *rw = (NvmeRwCmd *)&req->cmd;
    bool pi = !!NVME_ID_NS_DPS_TYPE(ns->id_ns.dps);
    bool pract = !!(le16_to_cpu(rw->control) & NVME_RW_PRINFO_PRACT);
    size_t len = nvme_l2b(ns, nlb);
    uint16_t status;

    if (nvme_ns_ext(ns) && !(pi && pract && ns->lbaf.ms == 8)) {
        NvmeSg sg;

        len += nvme_m2b(ns, nlb);

        status = nvme_map_dptr(n, &sg, len, &req->cmd);
        if (status) {
            return status;
        }

        nvme_sg_init(n, &req->sg, sg.flags & NVME_SG_DMA);
        nvme_sg_split(&sg, ns, &req->sg, NULL);
        nvme_sg_unmap(&sg);

        return NVME_SUCCESS;
    }

    return nvme_map_dptr(n, &req->sg, len, &req->cmd);
}