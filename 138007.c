static void nvme_init_pmr(NvmeCtrl *n, PCIDevice *pci_dev)
{
    uint32_t pmrcap = ldl_le_p(&n->bar.pmrcap);

    NVME_PMRCAP_SET_RDS(pmrcap, 1);
    NVME_PMRCAP_SET_WDS(pmrcap, 1);
    NVME_PMRCAP_SET_BIR(pmrcap, NVME_PMR_BIR);
    /* Turn on bit 1 support */
    NVME_PMRCAP_SET_PMRWBM(pmrcap, 0x02);
    NVME_PMRCAP_SET_CMSS(pmrcap, 1);
    stl_le_p(&n->bar.pmrcap, pmrcap);

    pci_register_bar(pci_dev, NVME_PMR_BIR,
                     PCI_BASE_ADDRESS_SPACE_MEMORY |
                     PCI_BASE_ADDRESS_MEM_TYPE_64 |
                     PCI_BASE_ADDRESS_MEM_PREFETCH, &n->pmr.dev->mr);

    memory_region_set_enabled(&n->pmr.dev->mr, false);
}