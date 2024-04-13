static bool nvme_addr_is_pmr(NvmeCtrl *n, hwaddr addr)
{
    hwaddr hi;

    if (!n->pmr.cmse) {
        return false;
    }

    hi = n->pmr.cba + int128_get64(n->pmr.dev->mr.size);

    return addr >= n->pmr.cba && addr < hi;
}