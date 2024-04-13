static uint16_t nvme_check_zone_state_for_read(NvmeZone *zone)
{
    switch (nvme_get_zone_state(zone)) {
    case NVME_ZONE_STATE_EMPTY:
    case NVME_ZONE_STATE_IMPLICITLY_OPEN:
    case NVME_ZONE_STATE_EXPLICITLY_OPEN:
    case NVME_ZONE_STATE_FULL:
    case NVME_ZONE_STATE_CLOSED:
    case NVME_ZONE_STATE_READ_ONLY:
        return NVME_SUCCESS;
    case NVME_ZONE_STATE_OFFLINE:
        trace_pci_nvme_err_zone_is_offline(zone->d.zslba);
        return NVME_ZONE_OFFLINE;
    default:
        assert(false);
    }

    return NVME_INTERNAL_DEV_ERROR;
}