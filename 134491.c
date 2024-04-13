static void megasas_scsi_reset(DeviceState *dev)
{
    MegasasState *s = MEGASAS(dev);

    megasas_soft_reset(s);
}