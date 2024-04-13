void esp_command_complete(SCSIRequest *req, size_t resid)
{
    ESPState *s = req->hba_private;

    trace_esp_command_complete();
    if (s->ti_size != 0) {
        trace_esp_command_complete_unexpected();
    }
    s->ti_size = 0;
    s->async_len = 0;
    if (req->status) {
        trace_esp_command_complete_fail();
    }
    s->status = req->status;
    s->rregs[ESP_RSTAT] = STAT_ST;
    esp_dma_done(s);
    esp_lower_drq(s);
    if (s->current_req) {
        scsi_req_unref(s->current_req);
        s->current_req = NULL;
        s->current_dev = NULL;
    }
}