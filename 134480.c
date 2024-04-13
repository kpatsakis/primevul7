static uint64_t megasas_mmio_read(void *opaque, hwaddr addr,
                                  unsigned size)
{
    MegasasState *s = opaque;
    PCIDevice *pci_dev = PCI_DEVICE(s);
    MegasasBaseClass *base_class = MEGASAS_DEVICE_GET_CLASS(s);
    uint32_t retval = 0;

    switch (addr) {
    case MFI_IDB:
        retval = 0;
        trace_megasas_mmio_readl("MFI_IDB", retval);
        break;
    case MFI_OMSG0:
    case MFI_OSP0:
        retval = (msix_present(pci_dev) ? MFI_FWSTATE_MSIX_SUPPORTED : 0) |
            (s->fw_state & MFI_FWSTATE_MASK) |
            ((s->fw_sge & 0xff) << 16) |
            (s->fw_cmds & 0xFFFF);
        trace_megasas_mmio_readl(addr == MFI_OMSG0 ? "MFI_OMSG0" : "MFI_OSP0",
                                 retval);
        break;
    case MFI_OSTS:
        if (megasas_intr_enabled(s) && s->doorbell) {
            retval = base_class->osts;
        }
        trace_megasas_mmio_readl("MFI_OSTS", retval);
        break;
    case MFI_OMSK:
        retval = s->intr_mask;
        trace_megasas_mmio_readl("MFI_OMSK", retval);
        break;
    case MFI_ODCR0:
        retval = s->doorbell ? 1 : 0;
        trace_megasas_mmio_readl("MFI_ODCR0", retval);
        break;
    case MFI_DIAG:
        retval = s->diag;
        trace_megasas_mmio_readl("MFI_DIAG", retval);
        break;
    case MFI_OSP1:
        retval = 15;
        trace_megasas_mmio_readl("MFI_OSP1", retval);
        break;
    default:
        trace_megasas_mmio_invalid_readl(addr);
        break;
    }
    return retval;
}