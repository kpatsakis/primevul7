static uint16_t nvme_set_zd_ext(NvmeNamespace *ns, NvmeZone *zone)
{
    uint16_t status;
    uint8_t state = nvme_get_zone_state(zone);

    if (state == NVME_ZONE_STATE_EMPTY) {
        status = nvme_aor_check(ns, 1, 0);
        if (status) {
            return status;
        }
        nvme_aor_inc_active(ns);
        zone->d.za |= NVME_ZA_ZD_EXT_VALID;
        nvme_assign_zone_state(ns, zone, NVME_ZONE_STATE_CLOSED);
        return NVME_SUCCESS;
    }

    return NVME_ZONE_INVAL_TRANSITION;
}