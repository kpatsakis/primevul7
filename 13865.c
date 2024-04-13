static int reds_agent_state_restore(RedsState *reds, SpiceMigrateDataMain *mig_data)
{
    RedCharDeviceVDIPort *agent_dev = reds->agent_dev.get();
    uint32_t chunk_header_remaining;

    agent_dev->priv->vdi_chunk_header = mig_data->agent2client.chunk_header;
    spice_assert(mig_data->agent2client.chunk_header_size <= sizeof(VDIChunkHeader));
    chunk_header_remaining = sizeof(VDIChunkHeader) - mig_data->agent2client.chunk_header_size;
    if (chunk_header_remaining) {
        agent_dev->priv->read_state = VDI_PORT_READ_STATE_READ_HEADER;
        agent_dev->priv->receive_pos = (uint8_t *)&agent_dev->priv->vdi_chunk_header +
            mig_data->agent2client.chunk_header_size;
        agent_dev->priv->receive_len = chunk_header_remaining;
    } else {
        agent_dev->priv->message_receive_len = agent_dev->priv->vdi_chunk_header.size;
    }

    if (!mig_data->agent2client.msg_header_done) {
        uint8_t *partial_msg_header;

        if (!chunk_header_remaining) {
            uint32_t cur_buf_size;

            agent_dev->priv->read_state = VDI_PORT_READ_STATE_READ_DATA;
            agent_dev->priv->current_read_buf = vdi_port_get_read_buf(agent_dev);
            spice_assert(agent_dev->priv->current_read_buf);
            partial_msg_header = (uint8_t *)mig_data + mig_data->agent2client.msg_header_ptr -
                sizeof(SpiceMiniDataHeader);
            memcpy(agent_dev->priv->current_read_buf->data,
                   partial_msg_header,
                   mig_data->agent2client.msg_header_partial_len);
            agent_dev->priv->receive_pos = agent_dev->priv->current_read_buf->data +
                                      mig_data->agent2client.msg_header_partial_len;
            cur_buf_size = sizeof(agent_dev->priv->current_read_buf->data) -
                           mig_data->agent2client.msg_header_partial_len;
            agent_dev->priv->receive_len = MIN(agent_dev->priv->message_receive_len, cur_buf_size);
            agent_dev->priv->current_read_buf->len = agent_dev->priv->receive_len +
                                                 mig_data->agent2client.msg_header_partial_len;
            agent_dev->priv->message_receive_len -= agent_dev->priv->receive_len;
        } else {
            spice_assert(mig_data->agent2client.msg_header_partial_len == 0);
        }
    } else {
            agent_dev->priv->read_state = VDI_PORT_READ_STATE_GET_BUFF;
            agent_dev->priv->current_read_buf.reset();
            agent_dev->priv->receive_pos = NULL;
            agent_dev->priv->read_filter.msg_data_to_read = mig_data->agent2client.msg_remaining;
            agent_dev->priv->read_filter.result = (AgentMsgFilterResult) mig_data->agent2client.msg_filter_result;
    }

    agent_dev->priv->read_filter.discard_all = FALSE;
    agent_dev->priv->write_filter.discard_all = !mig_data->client_agent_started;
    agent_dev->priv->client_agent_started = mig_data->client_agent_started;

    agent_dev->priv->write_filter.msg_data_to_read = mig_data->client2agent.msg_remaining;
    agent_dev->priv->write_filter.result = (AgentMsgFilterResult) mig_data->client2agent.msg_filter_result;

    spice_debug("to agent filter: discard all %d, wait_msg %u, msg_filter_result %d",
                agent_dev->priv->write_filter.discard_all,
                agent_dev->priv->write_filter.msg_data_to_read,
                agent_dev->priv->write_filter.result);
    spice_debug("from agent filter: discard all %d, wait_msg %u, msg_filter_result %d",
                agent_dev->priv->read_filter.discard_all,
                agent_dev->priv->read_filter.msg_data_to_read,
                agent_dev->priv->read_filter.result);
    return agent_dev->restore(&mig_data->agent_base);
}