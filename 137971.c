static uint16_t nvme_check_zone_write(NvmeNamespace *ns, NvmeZone *zone,
                                      uint64_t slba, uint32_t nlb)
{
    uint64_t zcap = nvme_zone_wr_boundary(zone);
    uint16_t status;

    status = nvme_check_zone_state_for_write(zone);
    if (status) {
        return status;
    }

    if (unlikely(slba != zone->w_ptr)) {
        trace_pci_nvme_err_write_not_at_wp(slba, zone->d.zslba, zone->w_ptr);
        return NVME_ZONE_INVALID_WRITE;
    }

    if (unlikely((slba + nlb) > zcap)) {
        trace_pci_nvme_err_zone_boundary(slba, nlb, zcap);
        return NVME_ZONE_BOUNDARY_ERROR;
    }

    return NVME_SUCCESS;
}