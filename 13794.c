uint8_t *reds_get_agent_data_buffer(RedsState *reds, MainChannelClient *mcc, size_t size)
{
    RedCharDeviceVDIPort *dev = reds->agent_dev.get();
    RedClient *client;

    if (!dev->priv->client_agent_started) {
        /*
         * agent got disconnected, and possibly got reconnected, but we still can receive
         * msgs that are addressed to the agent's old instance, in case they were
         * sent by the client before it received the AGENT_DISCONNECTED msg.
         * In such case, we will receive and discard the msgs (reds_reset_vdp takes care
         * of setting dev->write_filter.result = AGENT_MSG_FILTER_DISCARD).
         */
        return (uint8_t*) g_malloc(size);
    }

    spice_assert(dev->priv->recv_from_client_buf == NULL);
    client = mcc->get_client();
    dev->priv->recv_from_client_buf =
        dev->write_buffer_get_client((RedCharDeviceClientOpaque *)client,
                                     size + sizeof(VDIChunkHeader));
    /* check if buffer was allocated, as flow control is enabled for
     * this device this is a normal condition */
    if (!dev->priv->recv_from_client_buf) {
        return NULL;
    }
    dev->priv->recv_from_client_buf_pushed = FALSE;
    return dev->priv->recv_from_client_buf->buf + sizeof(VDIChunkHeader);
}