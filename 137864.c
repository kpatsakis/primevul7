static inline bool nvme_addr_is_dma(NvmeCtrl *n, hwaddr addr)
{
    return !(nvme_addr_is_cmb(n, addr) || nvme_addr_is_pmr(n, addr));
}