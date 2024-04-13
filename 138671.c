static void handle_ti(ESPState *s)
{
    uint32_t dmalen;

    if (s->dma && !s->dma_enabled) {
        s->dma_cb = handle_ti;
        return;
    }

    s->ti_cmd = s->rregs[ESP_CMD];
    if (s->dma) {
        dmalen = esp_get_tc(s);
        trace_esp_handle_ti(dmalen);
        s->rregs[ESP_RSTAT] &= ~STAT_TC;
        esp_do_dma(s);
    } else {
        trace_esp_handle_ti(s->ti_size);
        esp_do_nodma(s);
    }
}