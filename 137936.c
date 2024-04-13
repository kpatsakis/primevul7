static uint16_t nvme_open_zone(NvmeNamespace *ns, NvmeZone *zone,
                               NvmeZoneState state, NvmeRequest *req)
{
    return nvme_zrm_open(nvme_ctrl(req), ns, zone);
}