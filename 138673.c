static uint64_t sysbus_esp_mem_read(void *opaque, hwaddr addr,
                                    unsigned int size)
{
    SysBusESPState *sysbus = opaque;
    ESPState *s = ESP(&sysbus->esp);
    uint32_t saddr;

    saddr = addr >> sysbus->it_shift;
    return esp_reg_read(s, saddr);
}