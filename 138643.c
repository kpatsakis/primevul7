static void esp_pdma_write(ESPState *s, uint8_t val)
{
    uint32_t dmalen = esp_get_tc(s);

    if (dmalen == 0) {
        return;
    }

    if (s->do_cmd) {
        esp_fifo_push(&s->cmdfifo, val);
    } else {
        esp_fifo_push(&s->fifo, val);
    }

    dmalen--;
    esp_set_tc(s, dmalen);
}