static int nvme_init_pci(NvmeCtrl *n, PCIDevice *pci_dev, Error **errp)
{
    uint8_t *pci_conf = pci_dev->config;
    uint64_t bar_size, msix_table_size, msix_pba_size;
    unsigned msix_table_offset, msix_pba_offset;
    int ret;

    Error *err = NULL;

    pci_conf[PCI_INTERRUPT_PIN] = 1;
    pci_config_set_prog_interface(pci_conf, 0x2);

    if (n->params.use_intel_id) {
        pci_config_set_vendor_id(pci_conf, PCI_VENDOR_ID_INTEL);
        pci_config_set_device_id(pci_conf, 0x5845);
    } else {
        pci_config_set_vendor_id(pci_conf, PCI_VENDOR_ID_REDHAT);
        pci_config_set_device_id(pci_conf, PCI_DEVICE_ID_REDHAT_NVME);
    }

    pci_config_set_class(pci_conf, PCI_CLASS_STORAGE_EXPRESS);
    pcie_endpoint_cap_init(pci_dev, 0x80);

    bar_size = QEMU_ALIGN_UP(n->reg_size, 4 * KiB);
    msix_table_offset = bar_size;
    msix_table_size = PCI_MSIX_ENTRY_SIZE * n->params.msix_qsize;

    bar_size += msix_table_size;
    bar_size = QEMU_ALIGN_UP(bar_size, 4 * KiB);
    msix_pba_offset = bar_size;
    msix_pba_size = QEMU_ALIGN_UP(n->params.msix_qsize, 64) / 8;

    bar_size += msix_pba_size;
    bar_size = pow2ceil(bar_size);

    memory_region_init(&n->bar0, OBJECT(n), "nvme-bar0", bar_size);
    memory_region_init_io(&n->iomem, OBJECT(n), &nvme_mmio_ops, n, "nvme",
                          n->reg_size);
    memory_region_add_subregion(&n->bar0, 0, &n->iomem);

    pci_register_bar(pci_dev, 0, PCI_BASE_ADDRESS_SPACE_MEMORY |
                     PCI_BASE_ADDRESS_MEM_TYPE_64, &n->bar0);
    ret = msix_init(pci_dev, n->params.msix_qsize,
                    &n->bar0, 0, msix_table_offset,
                    &n->bar0, 0, msix_pba_offset, 0, &err);
    if (ret < 0) {
        if (ret == -ENOTSUP) {
            warn_report_err(err);
        } else {
            error_propagate(errp, err);
            return ret;
        }
    }

    if (n->params.cmb_size_mb) {
        nvme_init_cmb(n, pci_dev);
    }

    if (n->pmr.dev) {
        nvme_init_pmr(n, pci_dev);
    }

    return 0;
}