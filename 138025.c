static uint16_t nvme_close_zone(NvmeNamespace *ns, NvmeZone *zone,
                                NvmeZoneState state, NvmeRequest *req)
{
    return nvme_zrm_close(ns, zone);
}