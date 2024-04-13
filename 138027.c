static inline uint16_t nvme_zrm_open(NvmeCtrl *n, NvmeNamespace *ns,
                                     NvmeZone *zone)
{
    return nvme_zrm_open_flags(n, ns, zone, 0);
}