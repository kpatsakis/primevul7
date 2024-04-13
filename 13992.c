static AgentMsgFilterResult vdi_port_read_buf_process(RedCharDeviceVDIPort *dev,
                                                      RedVDIReadBuf& buf)
{
    switch (dev->priv->vdi_chunk_header.port) {
    case VDP_CLIENT_PORT:
        return agent_msg_filter_process_data(&dev->priv->read_filter, buf.data, buf.len);
    case VDP_SERVER_PORT:
        return AGENT_MSG_FILTER_DISCARD;
    default:
        spice_warning("invalid port");
        return AGENT_MSG_FILTER_PROTO_ERROR;
    }
}