void reds_on_main_channel_migrate(RedsState *reds, MainChannelClient *mcc)
{
    RedCharDeviceVDIPort *agent_dev = reds->agent_dev.get();
    uint32_t read_data_len;

    spice_assert(reds->clients.size() == 1);

    if (agent_dev->priv->read_state != VDI_PORT_READ_STATE_READ_DATA) {
        return;
    }
    spice_assert(agent_dev->priv->current_read_buf &&
                 agent_dev->priv->receive_pos > agent_dev->priv->current_read_buf->data);
    read_data_len = agent_dev->priv->receive_pos - agent_dev->priv->current_read_buf->data;

    if (agent_dev->priv->read_filter.msg_data_to_read ||
        read_data_len > sizeof(VDAgentMessage)) { /* msg header has been read */
        red::shared_ptr<RedVDIReadBuf> read_buf = std::move(agent_dev->priv->current_read_buf);

        spice_debug("push partial read %u (msg first chunk? %d)", read_data_len,
                    !agent_dev->priv->read_filter.msg_data_to_read);

        read_buf->len = read_data_len;
        switch (vdi_port_read_buf_process(agent_dev, *read_buf)) {
        case AGENT_MSG_FILTER_OK:
            reds_adjust_agent_capabilities(reds, (VDAgentMessage *)read_buf->data);
            mcc->push_agent_data(std::move(read_buf));
            break;
        case AGENT_MSG_FILTER_PROTO_ERROR:
            reds_agent_remove(reds);
            /* fall through */
        case AGENT_MSG_FILTER_MONITORS_CONFIG:
            /* fall through */
        case AGENT_MSG_FILTER_DISCARD:
            read_buf.reset();
        }

        spice_assert(agent_dev->priv->receive_len);
        agent_dev->priv->message_receive_len += agent_dev->priv->receive_len;
        agent_dev->priv->read_state = VDI_PORT_READ_STATE_GET_BUFF;
        agent_dev->priv->receive_pos = NULL;
    }
}