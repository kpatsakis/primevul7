static void sysbus_esp_pdma_write(void *opaque, hwaddr addr,
                                  uint64_t val, unsigned int size)
{
    SysBusESPState *sysbus = opaque;
    ESPState *s = ESP(&sysbus->esp);
    uint32_t dmalen;

    trace_esp_pdma_write(size);

    switch (size) {
    case 1:
        esp_pdma_write(s, val);
        break;
    case 2:
        esp_pdma_write(s, val >> 8);
        esp_pdma_write(s, val);
        break;
    }
    dmalen = esp_get_tc(s);
    if (dmalen == 0 || fifo8_num_free(&s->fifo) < 2) {
        s->pdma_cb(s);
    }
}