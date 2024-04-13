static void nvme_select_iocs(NvmeCtrl *n)
{
    NvmeNamespace *ns;
    int i;

    for (i = 1; i <= NVME_MAX_NAMESPACES; i++) {
        ns = nvme_ns(n, i);
        if (!ns) {
            continue;
        }

        nvme_select_iocs_ns(n, ns);
    }
}