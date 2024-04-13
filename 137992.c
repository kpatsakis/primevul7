static uint16_t nvme_finish_zone(NvmeNamespace *ns, NvmeZone *zone,
                                 NvmeZoneState state, NvmeRequest *req)
{
    return nvme_zrm_finish(ns, zone);
}