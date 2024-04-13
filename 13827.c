void RedCharDeviceVDIPort::on_free_self_token()
{
    RedsState *reds = get_server();

    if (reds->inputs_channel && reds->pending_mouse_event) {
        spice_debug("pending mouse event");
        reds_handle_agent_mouse_event(reds, reds->inputs_channel->get_mouse_state());
    }

    if (reds->pending_device_display_info_message) {
        spice_debug("pending device display info message");
        reds_send_device_display_info(reds);
    }
}