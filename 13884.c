static void reds_reset_vdp(RedsState *reds)
{
    RedCharDeviceVDIPort *dev = reds->agent_dev.get();
    SpiceCharDeviceInterface *sif;

    dev->priv->read_state = VDI_PORT_READ_STATE_READ_HEADER;
    dev->priv->receive_pos = (uint8_t *)&dev->priv->vdi_chunk_header;
    dev->priv->receive_len = sizeof(dev->priv->vdi_chunk_header);
    dev->priv->message_receive_len = 0;
    dev->priv->current_read_buf.reset();

    /* Reset read filter to start with clean state when the agent reconnects */
    agent_msg_filter_init(&dev->priv->read_filter, reds->config->agent_copypaste,
                          reds->config->agent_file_xfer,
                          reds_use_client_monitors_config(reds), TRUE);
    /* Throw away pending chunks from the current (if any) and future
     * messages written by the client.
     * TODO: client should clear its agent messages queue when the agent
     * is disconnected. Currently, when an agent gets disconnected and reconnected,
     * messages that were directed to the previous instance of the agent continue
     * to be sent from the client. This TODO will require server, protocol, and client changes */
    dev->priv->write_filter.result = AGENT_MSG_FILTER_DISCARD;
    dev->priv->write_filter.discard_all = TRUE;
    dev->priv->client_agent_started = false;
    dev->priv->agent_supports_graphics_device_info = false;

    /*  The client's tokens are set once when the main channel is initialized
     *  and once upon agent's connection with SPICE_MSG_MAIN_AGENT_CONNECTED_TOKENS.
     *  The client tokens are tracked as part of the RedCharDeviceClient. Thus,
     *  in order to be backward compatible with the client, we need to track the tokens
     *  even if the agent is detached. We don't destroy the char_device, and
     *  instead we just reset it.
     *  The tokens are also reset to avoid mismatch in upon agent reconnection.
     */
    dev->priv->agent_attached = FALSE;
    dev->stop();
    dev->reset();
    dev->reset_dev_instance(NULL);

    sif = spice_char_device_get_interface(reds->vdagent);
    if (sif->state) {
        sif->state(reds->vdagent, 0);
    }
}