static void nvme_init_cmb(NvmeCtrl *n, PCIDevice *pci_dev)
{
    uint64_t cmb_size = n->params.cmb_size_mb * MiB;
    uint64_t cap = ldq_le_p(&n->bar.cap);

    n->cmb.buf = g_malloc0(cmb_size);
    memory_region_init_io(&n->cmb.mem, OBJECT(n), &nvme_cmb_ops, n,
                          "nvme-cmb", cmb_size);
    pci_register_bar(pci_dev, NVME_CMB_BIR,
                     PCI_BASE_ADDRESS_SPACE_MEMORY |
                     PCI_BASE_ADDRESS_MEM_TYPE_64 |
                     PCI_BASE_ADDRESS_MEM_PREFETCH, &n->cmb.mem);

    NVME_CAP_SET_CMBS(cap, 1);
    stq_le_p(&n->bar.cap, cap);

    if (n->params.legacy_cmb) {
        nvme_cmb_enable_regs(n);
        n->cmb.cmse = true;
    }
}