static uint64_t megasas_frame_get_context(MegasasState *s,
                                          unsigned long frame)
{
    PCIDevice *pci = &s->parent_obj;
    return ldq_le_pci_dma(pci, frame + offsetof(struct mfi_frame_header, context));
}