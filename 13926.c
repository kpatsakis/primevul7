static void reds_adjust_agent_capabilities(RedsState *reds, VDAgentMessage *message)
{
    VDAgentAnnounceCapabilities *capabilities;

    if (message->type != VD_AGENT_ANNOUNCE_CAPABILITIES) {
        return;
    }
    capabilities = (VDAgentAnnounceCapabilities *) message->data;

    if (!reds->config->agent_copypaste) {
        VD_AGENT_CLEAR_CAPABILITY(capabilities->caps, VD_AGENT_CAP_CLIPBOARD);
        VD_AGENT_CLEAR_CAPABILITY(capabilities->caps, VD_AGENT_CAP_CLIPBOARD_BY_DEMAND);
        VD_AGENT_CLEAR_CAPABILITY(capabilities->caps, VD_AGENT_CAP_CLIPBOARD_SELECTION);
    }

    if (!reds->config->agent_file_xfer) {
        VD_AGENT_SET_CAPABILITY(capabilities->caps, VD_AGENT_CAP_FILE_XFER_DISABLED);
    }

    size_t caps_size = VD_AGENT_CAPS_SIZE_FROM_MSG_SIZE(message->size);
    reds->agent_dev->priv->agent_supports_graphics_device_info =
        VD_AGENT_HAS_CAPABILITY(capabilities->caps, caps_size, VD_AGENT_CAP_GRAPHICS_DEVICE_INFO);
    reds_send_device_display_info(reds);
}