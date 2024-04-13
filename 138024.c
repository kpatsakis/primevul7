static void nvme_advance_zone_wp(NvmeNamespace *ns, NvmeZone *zone,
                                 uint32_t nlb)
{
    zone->d.wp += nlb;

    if (zone->d.wp == nvme_zone_wr_boundary(zone)) {
        nvme_zrm_finish(ns, zone);
    }
}