void reds_handle_agent_mouse_event(RedsState *reds, const VDAgentMouseState *mouse_state)
{
    if (!reds->inputs_channel || !reds->agent_dev->priv->agent_attached) {
        return;
    }

    RedCharDeviceWriteBuffer *char_dev_buf = vdagent_new_write_buffer(reds->agent_dev.get(),
                                                                      VD_AGENT_MOUSE_STATE,
                                                                      sizeof(VDAgentMouseState),
                                                                      true);

    if (!char_dev_buf) {
        reds->pending_mouse_event = TRUE;
        return;
    }

    reds->pending_mouse_event = FALSE;

    VDInternalBuf *internal_buf = (VDInternalBuf *)char_dev_buf->buf;
    internal_buf->u.mouse_state = *mouse_state;

    reds->agent_dev->write_buffer_add(char_dev_buf);
}