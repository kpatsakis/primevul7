static inline void on_activating_ticketing(RedsState *reds)
{
    if (!reds->config->ticketing_enabled && reds_main_channel_connected(reds)) {
        spice_warning("disconnecting");
        reds_disconnect(reds);
    }
}