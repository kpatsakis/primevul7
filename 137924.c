static uint16_t nvme_check_zone_state_for_write(NvmeZone *zone)
{
    uint64_t zslba = zone->d.zslba;

    switch (nvme_get_zone_state(zone)) {
    case NVME_ZONE_STATE_EMPTY:
    case NVME_ZONE_STATE_IMPLICITLY_OPEN:
    case NVME_ZONE_STATE_EXPLICITLY_OPEN:
    case NVME_ZONE_STATE_CLOSED:
        return NVME_SUCCESS;
    case NVME_ZONE_STATE_FULL:
        trace_pci_nvme_err_zone_is_full(zslba);
        return NVME_ZONE_FULL;
    case NVME_ZONE_STATE_OFFLINE:
        trace_pci_nvme_err_zone_is_offline(zslba);
        return NVME_ZONE_OFFLINE;
    case NVME_ZONE_STATE_READ_ONLY:
        trace_pci_nvme_err_zone_is_read_only(zslba);
        return NVME_ZONE_READ_ONLY;
    default:
        assert(false);
    }

    return NVME_INTERNAL_DEV_ERROR;
}