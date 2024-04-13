static uint16_t nvme_zrm_finish(NvmeNamespace *ns, NvmeZone *zone)
{
    switch (nvme_get_zone_state(zone)) {
    case NVME_ZONE_STATE_FULL:
        return NVME_SUCCESS;

    case NVME_ZONE_STATE_IMPLICITLY_OPEN:
    case NVME_ZONE_STATE_EXPLICITLY_OPEN:
        nvme_aor_dec_open(ns);
        /* fallthrough */
    case NVME_ZONE_STATE_CLOSED:
        nvme_aor_dec_active(ns);
        /* fallthrough */
    case NVME_ZONE_STATE_EMPTY:
        nvme_assign_zone_state(ns, zone, NVME_ZONE_STATE_FULL);
        return NVME_SUCCESS;

    default:
        return NVME_ZONE_INVAL_TRANSITION;
    }
}