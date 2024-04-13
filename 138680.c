static uint32_t esp_get_stc(ESPState *s)
{
    uint32_t dmalen;

    dmalen = s->wregs[ESP_TCLO];
    dmalen |= s->wregs[ESP_TCMID] << 8;
    dmalen |= s->wregs[ESP_TCHI] << 16;

    return dmalen;
}