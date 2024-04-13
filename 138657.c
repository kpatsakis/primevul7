static void handle_satn_stop(ESPState *s)
{
    int32_t cmdlen;

    if (s->dma && !s->dma_enabled) {
        s->dma_cb = handle_satn_stop;
        return;
    }
    s->pdma_cb = satn_stop_pdma_cb;
    cmdlen = get_cmd(s, 1);
    if (cmdlen > 0) {
        trace_esp_handle_satn_stop(fifo8_num_used(&s->cmdfifo));
        s->do_cmd = 1;
        s->cmdfifo_cdb_offset = 1;
        s->rregs[ESP_RSTAT] = STAT_MO;
        s->rregs[ESP_RINTR] |= INTR_BS | INTR_FC;
        s->rregs[ESP_RSEQ] = SEQ_MO;
        esp_raise_irq(s);
    } else if (cmdlen == 0) {
        s->do_cmd = 1;
        /* Target present, switch to message out phase */
        s->rregs[ESP_RSEQ] = SEQ_MO;
        s->rregs[ESP_RSTAT] = STAT_MO;
    }
}