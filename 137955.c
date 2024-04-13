static uint16_t nvme_check_zone_read(NvmeNamespace *ns, uint64_t slba,
                                     uint32_t nlb)
{
    NvmeZone *zone;
    uint64_t bndry, end;
    uint16_t status;

    zone = nvme_get_zone_by_slba(ns, slba);
    assert(zone);

    bndry = nvme_zone_rd_boundary(ns, zone);
    end = slba + nlb;

    status = nvme_check_zone_state_for_read(zone);
    if (status) {
        ;
    } else if (unlikely(end > bndry)) {
        if (!ns->params.cross_zone_read) {
            status = NVME_ZONE_BOUNDARY_ERROR;
        } else {
            /*
             * Read across zone boundary - check that all subsequent
             * zones that are being read have an appropriate state.
             */
            do {
                zone++;
                status = nvme_check_zone_state_for_read(zone);
                if (status) {
                    break;
                }
            } while (end > nvme_zone_rd_boundary(ns, zone));
        }
    }

    return status;
}