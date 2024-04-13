static bool nvme_addr_is_cmb(NvmeCtrl *n, hwaddr addr)
{
    hwaddr hi, lo;

    if (!n->cmb.cmse) {
        return false;
    }

    lo = n->params.legacy_cmb ? n->cmb.mem.addr : n->cmb.cba;
    hi = lo + int128_get64(n->cmb.mem.size);

    return addr >= lo && addr < hi;
}