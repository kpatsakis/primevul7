RedCharDeviceVDIPort::RedCharDeviceVDIPort(RedsState *reds):
    RedCharDevice(reds, nullptr, REDS_TOKENS_TO_SEND, REDS_NUM_INTERNAL_AGENT_MESSAGES)
{
    priv->read_state = VDI_PORT_READ_STATE_READ_HEADER;
    priv->receive_pos = (uint8_t *)&priv->vdi_chunk_header;
    priv->receive_len = sizeof(priv->vdi_chunk_header);

    RedCharDeviceVDIPort *dev = this;

    agent_msg_filter_init(&dev->priv->write_filter, reds->config->agent_copypaste,
                          reds->config->agent_file_xfer,
                          reds_use_client_monitors_config(reds),
                          TRUE);
    agent_msg_filter_init(&dev->priv->read_filter, reds->config->agent_copypaste,
                          reds->config->agent_file_xfer,
                          reds_use_client_monitors_config(reds),
                          TRUE);
}