static void sysbus_esp_realize(DeviceState *dev, Error **errp)
{
    SysBusDevice *sbd = SYS_BUS_DEVICE(dev);
    SysBusESPState *sysbus = SYSBUS_ESP(dev);
    ESPState *s = ESP(&sysbus->esp);

    if (!qdev_realize(DEVICE(s), NULL, errp)) {
        return;
    }

    sysbus_init_irq(sbd, &s->irq);
    sysbus_init_irq(sbd, &s->irq_data);
    assert(sysbus->it_shift != -1);

    s->chip_id = TCHI_FAS100A;
    memory_region_init_io(&sysbus->iomem, OBJECT(sysbus), &sysbus_esp_mem_ops,
                          sysbus, "esp-regs", ESP_REGS << sysbus->it_shift);
    sysbus_init_mmio(sbd, &sysbus->iomem);
    memory_region_init_io(&sysbus->pdma, OBJECT(sysbus), &sysbus_esp_pdma_ops,
                          sysbus, "esp-pdma", 4);
    sysbus_init_mmio(sbd, &sysbus->pdma);

    qdev_init_gpio_in(dev, sysbus_esp_gpio_demux, 2);

    scsi_bus_new(&s->bus, sizeof(s->bus), dev, &esp_scsi_info, NULL);
}