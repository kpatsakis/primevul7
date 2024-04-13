static uint16_t nvme_map_mdata(NvmeCtrl *n, uint32_t nlb, NvmeRequest *req)
{
    NvmeNamespace *ns = req->ns;
    size_t len = nvme_m2b(ns, nlb);
    uint16_t status;

    if (nvme_ns_ext(ns)) {
        NvmeSg sg;

        len += nvme_l2b(ns, nlb);

        status = nvme_map_dptr(n, &sg, len, &req->cmd);
        if (status) {
            return status;
        }

        nvme_sg_init(n, &req->sg, sg.flags & NVME_SG_DMA);
        nvme_sg_split(&sg, ns, NULL, &req->sg);
        nvme_sg_unmap(&sg);

        return NVME_SUCCESS;
    }

    return nvme_map_mptr(n, &req->sg, len, &req->cmd);
}