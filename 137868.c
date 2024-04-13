static inline void *nvme_addr_to_pmr(NvmeCtrl *n, hwaddr addr)
{
    return memory_region_get_ram_ptr(&n->pmr.dev->mr) + (addr - n->pmr.cba);
}