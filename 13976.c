void reds_handle_channel_event(RedsState *reds, int event, SpiceChannelEventInfo *info)
{
    reds->core.channel_event(&reds->core, event, info);

    if (event == SPICE_CHANNEL_EVENT_DISCONNECTED) {
        g_free(info);
    }
}