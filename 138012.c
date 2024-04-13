static uint16_t nvme_zrm_reset(NvmeNamespace *ns, NvmeZone *zone)
{
    switch (nvme_get_zone_state(zone)) {
    case NVME_ZONE_STATE_EXPLICITLY_OPEN:
    case NVME_ZONE_STATE_IMPLICITLY_OPEN:
        nvme_aor_dec_open(ns);
        /* fallthrough */
    case NVME_ZONE_STATE_CLOSED:
        nvme_aor_dec_active(ns);
        /* fallthrough */
    case NVME_ZONE_STATE_FULL:
        zone->w_ptr = zone->d.zslba;
        zone->d.wp = zone->w_ptr;
        nvme_assign_zone_state(ns, zone, NVME_ZONE_STATE_EMPTY);
        /* fallthrough */
    case NVME_ZONE_STATE_EMPTY:
        return NVME_SUCCESS;

    default:
        return NVME_ZONE_INVAL_TRANSITION;
    }
}