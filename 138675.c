static void sysbus_esp_hard_reset(DeviceState *dev)
{
    SysBusESPState *sysbus = SYSBUS_ESP(dev);
    ESPState *s = ESP(&sysbus->esp);

    esp_hard_reset(s);
}