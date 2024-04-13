static void megasas_class_init(ObjectClass *oc, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(oc);
    PCIDeviceClass *pc = PCI_DEVICE_CLASS(oc);
    MegasasBaseClass *e = MEGASAS_DEVICE_CLASS(oc);
    const MegasasInfo *info = data;

    pc->realize = megasas_scsi_realize;
    pc->exit = megasas_scsi_uninit;
    pc->vendor_id = PCI_VENDOR_ID_LSI_LOGIC;
    pc->device_id = info->device_id;
    pc->subsystem_vendor_id = PCI_VENDOR_ID_LSI_LOGIC;
    pc->subsystem_id = info->subsystem_id;
    pc->class_id = PCI_CLASS_STORAGE_RAID;
    pc->is_express = info->is_express;
    e->mmio_bar = info->mmio_bar;
    e->ioport_bar = info->ioport_bar;
    e->osts = info->osts;
    e->product_name = info->product_name;
    e->product_version = info->product_version;
    dc->props = info->props;
    dc->reset = megasas_scsi_reset;
    dc->vmsd = info->vmsd;
    set_bit(DEVICE_CATEGORY_STORAGE, dc->categories);
    dc->desc = info->desc;
}