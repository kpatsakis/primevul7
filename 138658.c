static uint64_t sysbus_esp_pdma_read(void *opaque, hwaddr addr,
                                     unsigned int size)
{
    SysBusESPState *sysbus = opaque;
    ESPState *s = ESP(&sysbus->esp);
    uint64_t val = 0;

    trace_esp_pdma_read(size);

    switch (size) {
    case 1:
        val = esp_pdma_read(s);
        break;
    case 2:
        val = esp_pdma_read(s);
        val = (val << 8) | esp_pdma_read(s);
        break;
    }
    if (fifo8_num_used(&s->fifo) < 2) {
        s->pdma_cb(s);
    }
    return val;
}