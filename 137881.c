static inline NvmeZone *nvme_get_zone_by_slba(NvmeNamespace *ns, uint64_t slba)
{
    uint32_t zone_idx = nvme_zone_idx(ns, slba);

    if (zone_idx >= ns->num_zones) {
        return NULL;
    }

    return &ns->zone_array[zone_idx];
}