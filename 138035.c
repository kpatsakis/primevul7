static inline void *nvme_addr_to_cmb(NvmeCtrl *n, hwaddr addr)
{
    hwaddr base = n->params.legacy_cmb ? n->cmb.mem.addr : n->cmb.cba;
    return &n->cmb.buf[addr - base];
}