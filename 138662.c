static void esp_set_tc(ESPState *s, uint32_t dmalen)
{
    s->rregs[ESP_TCLO] = dmalen;
    s->rregs[ESP_TCMID] = dmalen >> 8;
    s->rregs[ESP_TCHI] = dmalen >> 16;
}