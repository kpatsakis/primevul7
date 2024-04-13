static void nvme_exit(PCIDevice *pci_dev)
{
    NvmeCtrl *n = NVME(pci_dev);
    NvmeNamespace *ns;
    int i;

    nvme_ctrl_reset(n);

    if (n->subsys) {
        for (i = 1; i <= NVME_MAX_NAMESPACES; i++) {
            ns = nvme_ns(n, i);
            if (ns) {
                ns->attached--;
            }
        }

        nvme_subsys_unregister_ctrl(n->subsys, n);
    }

    g_free(n->cq);
    g_free(n->sq);
    g_free(n->aer_reqs);

    if (n->params.cmb_size_mb) {
        g_free(n->cmb.buf);
    }

    if (n->pmr.dev) {
        host_memory_backend_set_mapped(n->pmr.dev, false);
    }
    msix_uninit(pci_dev, &n->bar0, &n->bar0);
    memory_region_del_subregion(&n->bar0, &n->iomem);
}