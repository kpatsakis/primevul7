void reds_on_main_agent_data(RedsState *reds, MainChannelClient *mcc, const void *message,
                             size_t size)
{
    RedCharDeviceVDIPort *dev = reds->agent_dev.get();
    VDIChunkHeader *header;
    AgentMsgFilterResult res;

    res = agent_msg_filter_process_data(&dev->priv->write_filter,
                                        (const uint8_t*) message, size);
    switch (res) {
    case AGENT_MSG_FILTER_OK:
        break;
    case AGENT_MSG_FILTER_DISCARD:
        return;
    case AGENT_MSG_FILTER_MONITORS_CONFIG:
        reds_on_main_agent_monitors_config(reds, mcc, message, size);
        return;
    case AGENT_MSG_FILTER_PROTO_ERROR:
        mcc->shutdown();
        return;
    }

    spice_assert(dev->priv->recv_from_client_buf);
    spice_assert(message == dev->priv->recv_from_client_buf->buf + sizeof(VDIChunkHeader));
    // TODO - start tracking agent data per channel
    header =  (VDIChunkHeader *)dev->priv->recv_from_client_buf->buf;
    header->port = VDP_CLIENT_PORT;
    header->size = size;
    dev->priv->recv_from_client_buf->buf_used = sizeof(VDIChunkHeader) + size;

    dev->priv->recv_from_client_buf_pushed = TRUE;
    dev->write_buffer_add(dev->priv->recv_from_client_buf);
}