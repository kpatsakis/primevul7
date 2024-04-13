static void nvme_ctrl_shutdown(NvmeCtrl *n)
{
    NvmeNamespace *ns;
    int i;

    if (n->pmr.dev) {
        memory_region_msync(&n->pmr.dev->mr, 0, n->pmr.dev->size);
    }

    for (i = 1; i <= NVME_MAX_NAMESPACES; i++) {
        ns = nvme_ns(n, i);
        if (!ns) {
            continue;
        }

        nvme_ns_shutdown(ns);
    }
}