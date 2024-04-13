void reds_send_device_display_info(RedsState *reds)
{
    if (!reds->agent_dev->priv->agent_attached) {
        return;
    }
    if (!reds->agent_dev->priv->agent_supports_graphics_device_info) {
        return;
    }

    g_debug("Sending device display info to the agent:");

    SpiceMarshaller *m = spice_marshaller_new();
    reds_marshall_device_display_info(reds, m);

    RedCharDeviceWriteBuffer *char_dev_buf = vdagent_new_write_buffer(reds->agent_dev.get(),
                                         VD_AGENT_GRAPHICS_DEVICE_INFO,
                                         spice_marshaller_get_total_size(m),
                                         true);

    if (!char_dev_buf) {
        spice_marshaller_destroy(m);
        reds->pending_device_display_info_message = true;
        return;
    }

    VDInternalBuf *internal_buf = (VDInternalBuf *)char_dev_buf->buf;

    int free_info;
    size_t len_info;
    uint8_t *info = spice_marshaller_linearize(m, 0, &len_info, &free_info);
    memcpy(&internal_buf->u.graphics_device_info, info, len_info);
    if (free_info) {
        free(info);
    }
    spice_marshaller_destroy(m);

    reds->pending_device_display_info_message = false;

    reds->agent_dev->write_buffer_add(char_dev_buf);
}