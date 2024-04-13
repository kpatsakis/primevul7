static uint32_t esp_get_tc(ESPState *s)
{
    uint32_t dmalen;

    dmalen = s->rregs[ESP_TCLO];
    dmalen |= s->rregs[ESP_TCMID] << 8;
    dmalen |= s->rregs[ESP_TCHI] << 16;

    return dmalen;
}