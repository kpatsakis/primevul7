static int esp_post_load(void *opaque, int version_id)
{
    ESPState *s = ESP(opaque);
    int len, i;

    version_id = MIN(version_id, s->mig_version_id);

    if (version_id < 5) {
        esp_set_tc(s, s->mig_dma_left);

        /* Migrate ti_buf to fifo */
        len = s->mig_ti_wptr - s->mig_ti_rptr;
        for (i = 0; i < len; i++) {
            fifo8_push(&s->fifo, s->mig_ti_buf[i]);
        }

        /* Migrate cmdbuf to cmdfifo */
        for (i = 0; i < s->mig_cmdlen; i++) {
            fifo8_push(&s->cmdfifo, s->mig_cmdbuf[i]);
        }
    }

    s->mig_version_id = vmstate_esp.version_id;
    return 0;
}