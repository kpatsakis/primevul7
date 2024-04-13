static void megasas_frame_set_cmd_status(MegasasState *s,
                                         unsigned long frame, uint8_t v)
{
    PCIDevice *pci = &s->parent_obj;
    stb_pci_dma(pci, frame + offsetof(struct mfi_frame_header, cmd_status), v);
}