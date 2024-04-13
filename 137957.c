static void nvme_finalize_zoned_write(NvmeNamespace *ns, NvmeRequest *req)
{
    NvmeRwCmd *rw = (NvmeRwCmd *)&req->cmd;
    NvmeZone *zone;
    uint64_t slba;
    uint32_t nlb;

    slba = le64_to_cpu(rw->slba);
    nlb = le16_to_cpu(rw->nlb) + 1;
    zone = nvme_get_zone_by_slba(ns, slba);
    assert(zone);

    nvme_advance_zone_wp(ns, zone, nlb);
}