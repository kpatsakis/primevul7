void reds_set_client_mm_time_latency(RedsState *reds, RedClient *client, uint32_t latency)
{
    // TODO: multi-client support for mm_time
    if (reds->mm_time_enabled) {
        // TODO: consider network latency
        if (latency > reds->mm_time_latency) {
            reds->mm_time_latency = latency;
            reds_send_mm_time(reds);
        } else {
            spice_debug("new latency %u is smaller than existing %u",
                        latency, reds->mm_time_latency);
        }
    } else {
        snd_set_playback_latency(client, latency);
    }
}