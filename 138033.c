static uint16_t nvme_zrm_open_flags(NvmeCtrl *n, NvmeNamespace *ns,
                                    NvmeZone *zone, int flags)
{
    int act = 0;
    uint16_t status;

    switch (nvme_get_zone_state(zone)) {
    case NVME_ZONE_STATE_EMPTY:
        act = 1;

        /* fallthrough */

    case NVME_ZONE_STATE_CLOSED:
        if (n->params.auto_transition_zones) {
            nvme_zrm_auto_transition_zone(ns);
        }
        status = nvme_aor_check(ns, act, 1);
        if (status) {
            return status;
        }

        if (act) {
            nvme_aor_inc_active(ns);
        }

        nvme_aor_inc_open(ns);

        if (flags & NVME_ZRM_AUTO) {
            nvme_assign_zone_state(ns, zone, NVME_ZONE_STATE_IMPLICITLY_OPEN);
            return NVME_SUCCESS;
        }

        /* fallthrough */

    case NVME_ZONE_STATE_IMPLICITLY_OPEN:
        if (flags & NVME_ZRM_AUTO) {
            return NVME_SUCCESS;
        }

        nvme_assign_zone_state(ns, zone, NVME_ZONE_STATE_EXPLICITLY_OPEN);

        /* fallthrough */

    case NVME_ZONE_STATE_EXPLICITLY_OPEN:
        return NVME_SUCCESS;

    default:
        return NVME_ZONE_INVAL_TRANSITION;
    }
}