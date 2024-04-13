static void sysbus_esp_mem_write(void *opaque, hwaddr addr,
                                 uint64_t val, unsigned int size)
{
    SysBusESPState *sysbus = opaque;
    ESPState *s = ESP(&sysbus->esp);
    uint32_t saddr;

    saddr = addr >> sysbus->it_shift;
    esp_reg_write(s, saddr, val);
}