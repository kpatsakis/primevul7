static void write_response_pdma_cb(ESPState *s)
{
    s->rregs[ESP_RSTAT] = STAT_TC | STAT_ST;
    s->rregs[ESP_RINTR] |= INTR_BS | INTR_FC;
    s->rregs[ESP_RSEQ] = SEQ_CD;
    esp_raise_irq(s);
}