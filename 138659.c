static void satn_pdma_cb(ESPState *s)
{
    s->do_cmd = 0;
    if (!fifo8_is_empty(&s->cmdfifo)) {
        s->cmdfifo_cdb_offset = 1;
        do_cmd(s);
    }
}