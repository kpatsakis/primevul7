static void reds_update_mouse_mode(RedsState *reds)
{
    int allowed = 0;
    int qxl_count = reds->qxl_instances.size();
    int display_channel_count = 0;

    for (const auto channel: reds->channels) {
        if (channel->type() == SPICE_CHANNEL_DISPLAY) {
            ++display_channel_count;
        }
    }

    if ((reds->config->agent_mouse && reds->vdagent) ||
        (reds->inputs_channel && reds->inputs_channel->has_tablet() &&
            qxl_count == 1 && display_channel_count == 1)) {
        allowed = reds->dispatcher_allows_client_mouse;
    }
    if (allowed == reds->is_client_mouse_allowed) {
        return;
    }
    reds->is_client_mouse_allowed = allowed;
    if (reds->mouse_mode == SPICE_MOUSE_MODE_CLIENT && !allowed) {
        reds_set_mouse_mode(reds, SPICE_MOUSE_MODE_SERVER);
        return;
    }
    if (reds->main_channel) {
        reds->main_channel->push_mouse_mode(reds->mouse_mode,
                                            reds->is_client_mouse_allowed);
    }
}