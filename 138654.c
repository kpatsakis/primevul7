static void satn_stop_pdma_cb(ESPState *s)
{
    s->do_cmd = 0;
    if (!fifo8_is_empty(&s->cmdfifo)) {
        trace_esp_handle_satn_stop(fifo8_num_used(&s->cmdfifo));
        s->do_cmd = 1;
        s->cmdfifo_cdb_offset = 1;
        s->rregs[ESP_RSTAT] = STAT_TC | STAT_CD;
        s->rregs[ESP_RINTR] |= INTR_BS | INTR_FC;
        s->rregs[ESP_RSEQ] = SEQ_CD;
        esp_raise_irq(s);
    }
}