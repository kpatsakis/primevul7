static inline uint16_t nvme_zrm_auto(NvmeCtrl *n, NvmeNamespace *ns,
                                     NvmeZone *zone)
{
    return nvme_zrm_open_flags(n, ns, zone, NVME_ZRM_AUTO);
}