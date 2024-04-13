static void s_without_satn_pdma_cb(ESPState *s)
{
    uint32_t len;

    s->do_cmd = 0;
    len = fifo8_num_used(&s->cmdfifo);
    if (len) {
        s->cmdfifo_cdb_offset = 0;
        do_busid_cmd(s, 0);
    }
}