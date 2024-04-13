void reds_marshall_migrate_data(RedsState *reds, SpiceMarshaller *m)
{
    SpiceMigrateDataMain mig_data;
    RedCharDeviceVDIPort *agent_dev = reds->agent_dev.get();
    SpiceMarshaller *m2;

    memset(&mig_data, 0, sizeof(mig_data));
    spice_marshaller_add_uint32(m, SPICE_MIGRATE_DATA_MAIN_MAGIC);
    spice_marshaller_add_uint32(m, SPICE_MIGRATE_DATA_MAIN_VERSION);

    if (!reds->vdagent) {
        uint8_t *null_agent_mig_data;

        /* MSG_AGENT_CONNECTED_TOKENS is supported by the client
           (see spice_server_migrate_connect), so agent_attached
           is set to FALSE when the agent is disconnected and
           there is no need to track the client tokens
           (see reds_reset_vdp) */
        spice_assert(!agent_dev->priv->agent_attached);
        RedCharDevice::migrate_data_marshall_empty(m);
        size_t padding_len = sizeof(SpiceMigrateDataMain) - sizeof(SpiceMigrateDataCharDevice);
        null_agent_mig_data = spice_marshaller_reserve_space(m, padding_len);
        memset(null_agent_mig_data, 0, padding_len);
        return;
    }

    agent_dev->migrate_data_marshall(m);
    spice_marshaller_add_uint8(m, agent_dev->priv->client_agent_started);

    mig_data.agent2client.chunk_header = agent_dev->priv->vdi_chunk_header;

    /* agent to client partial msg */
    if (agent_dev->priv->read_state == VDI_PORT_READ_STATE_READ_HEADER) {
        mig_data.agent2client.chunk_header_size = agent_dev->priv->receive_pos -
            (uint8_t *)&agent_dev->priv->vdi_chunk_header;

        mig_data.agent2client.msg_header_done = FALSE;
        mig_data.agent2client.msg_header_partial_len = 0;
        spice_assert(!agent_dev->priv->read_filter.msg_data_to_read);
    } else {
        mig_data.agent2client.chunk_header_size = sizeof(VDIChunkHeader);
        mig_data.agent2client.chunk_header.size = agent_dev->priv->message_receive_len;
        if (agent_dev->priv->read_state == VDI_PORT_READ_STATE_READ_DATA) {
            /* in the middle of reading the message header (see reds_on_main_channel_migrate) */
            mig_data.agent2client.msg_header_done = FALSE;
            mig_data.agent2client.msg_header_partial_len =
                agent_dev->priv->receive_pos - agent_dev->priv->current_read_buf->data;
            spice_assert(mig_data.agent2client.msg_header_partial_len < sizeof(VDAgentMessage));
            spice_assert(!agent_dev->priv->read_filter.msg_data_to_read);
        } else {
            mig_data.agent2client.msg_header_done =  TRUE;
            mig_data.agent2client.msg_remaining = agent_dev->priv->read_filter.msg_data_to_read;
            mig_data.agent2client.msg_filter_result = agent_dev->priv->read_filter.result;
        }
    }
    spice_marshaller_add_uint32(m, mig_data.agent2client.chunk_header_size);
    spice_marshaller_add(m,
                         (uint8_t *)&mig_data.agent2client.chunk_header,
                         sizeof(VDIChunkHeader));
    spice_marshaller_add_uint8(m, mig_data.agent2client.msg_header_done);
    spice_marshaller_add_uint32(m, mig_data.agent2client.msg_header_partial_len);
    m2 = spice_marshaller_get_ptr_submarshaller(m);
    spice_marshaller_add(m2, agent_dev->priv->current_read_buf->data,
                         mig_data.agent2client.msg_header_partial_len);
    spice_marshaller_add_uint32(m, mig_data.agent2client.msg_remaining);
    spice_marshaller_add_uint8(m, mig_data.agent2client.msg_filter_result);

    mig_data.client2agent.msg_remaining = agent_dev->priv->write_filter.msg_data_to_read;
    mig_data.client2agent.msg_filter_result = agent_dev->priv->write_filter.result;
    spice_marshaller_add_uint32(m, mig_data.client2agent.msg_remaining);
    spice_marshaller_add_uint8(m, mig_data.client2agent.msg_filter_result);
    spice_debug("from agent filter: discard all %d, wait_msg %u, msg_filter_result %d",
                agent_dev->priv->read_filter.discard_all,
                agent_dev->priv->read_filter.msg_data_to_read,
                agent_dev->priv->read_filter.result);
    spice_debug("to agent filter: discard all %d, wait_msg %u, msg_filter_result %d",
                agent_dev->priv->write_filter.discard_all,
                agent_dev->priv->write_filter.msg_data_to_read,
                agent_dev->priv->write_filter.result);
}