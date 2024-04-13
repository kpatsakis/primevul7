void esp_transfer_data(SCSIRequest *req, uint32_t len)
{
    ESPState *s = req->hba_private;
    int to_device = ((s->rregs[ESP_RSTAT] & 7) == STAT_DO);
    uint32_t dmalen = esp_get_tc(s);

    assert(!s->do_cmd);
    trace_esp_transfer_data(dmalen, s->ti_size);
    s->async_len = len;
    s->async_buf = scsi_req_get_buf(req);

    if (!to_device && !s->data_in_ready) {
        /*
         * Initial incoming data xfer is complete so raise command
         * completion interrupt
         */
        s->data_in_ready = true;
        s->rregs[ESP_RSTAT] |= STAT_TC;
        s->rregs[ESP_RINTR] |= INTR_BS;
        esp_raise_irq(s);

        /*
         * If data is ready to transfer and the TI command has already
         * been executed, start DMA immediately. Otherwise DMA will start
         * when host sends the TI command
         */
        if (s->ti_size && (s->rregs[ESP_CMD] == (CMD_TI | CMD_DMA))) {
            esp_do_dma(s);
        }
        return;
    }

    if (s->ti_cmd == 0) {
        /*
         * Always perform the initial transfer upon reception of the next TI
         * command to ensure the DMA/non-DMA status of the command is correct.
         * It is not possible to use s->dma directly in the section below as
         * some OSs send non-DMA NOP commands after a DMA transfer. Hence if the
         * async data transfer is delayed then s->dma is set incorrectly.
         */
        return;
    }

    if (s->ti_cmd & CMD_DMA) {
        if (dmalen) {
            esp_do_dma(s);
        } else if (s->ti_size <= 0) {
            /*
             * If this was the last part of a DMA transfer then the
             * completion interrupt is deferred to here.
             */
            esp_dma_done(s);
            esp_lower_drq(s);
        }
    } else {
        esp_do_nodma(s);
    }
}