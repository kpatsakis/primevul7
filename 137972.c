static uint16_t nvme_zrm_close(NvmeNamespace *ns, NvmeZone *zone)
{
    switch (nvme_get_zone_state(zone)) {
    case NVME_ZONE_STATE_EXPLICITLY_OPEN:
    case NVME_ZONE_STATE_IMPLICITLY_OPEN:
        nvme_aor_dec_open(ns);
        nvme_assign_zone_state(ns, zone, NVME_ZONE_STATE_CLOSED);
        /* fall through */
    case NVME_ZONE_STATE_CLOSED:
        return NVME_SUCCESS;

    default:
        return NVME_ZONE_INVAL_TRANSITION;
    }
}