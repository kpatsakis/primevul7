static void reds_send_mm_time(RedsState *reds)
{
    if (!reds_main_channel_connected(reds)) {
        return;
    }
    spice_debug("trace");
    reds->main_channel->push_multi_media_time(reds_get_mm_time() - reds->mm_time_latency);
}