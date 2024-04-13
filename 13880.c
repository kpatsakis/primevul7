static void reds_on_main_agent_monitors_config(RedsState *reds,
        MainChannelClient *mcc, const void *message, size_t size)
{
    const unsigned int MAX_NUM_MONITORS = 256;
    const unsigned int MAX_MONITOR_CONFIG_SIZE =
        sizeof(VDAgentMonitorsConfig) +
        MAX_NUM_MONITORS * (sizeof(VDAgentMonConfig) + sizeof(VDAgentMonitorMM));

    VDAgentMessage *msg_header;
    VDAgentMonitorsConfig *monitors_config;
    SpiceBuffer *cmc = &reds->client_monitors_config;
    uint32_t msg_size;

    // limit size of message sent by the client as this can cause a DoS through
    // memory exhaustion, or potentially some integer overflows
    if (sizeof(VDAgentMessage) + MAX_MONITOR_CONFIG_SIZE - cmc->offset < size) {
        goto overflow;
    }
    spice_buffer_append(cmc, message, size);
    if (sizeof(VDAgentMessage) > cmc->offset) {
        spice_debug("not enough data yet. %" G_GSSIZE_FORMAT, cmc->offset);
        return;
    }
    msg_header = (VDAgentMessage *)cmc->buffer;
    msg_size = GUINT32_FROM_LE(msg_header->size);
    if (msg_size > MAX_MONITOR_CONFIG_SIZE) {
        goto overflow;
    }
    if (msg_size > cmc->offset - sizeof(VDAgentMessage)) {
        spice_debug("not enough data yet. %" G_GSSIZE_FORMAT, cmc->offset);
        return;
    }

    // convert VDAgentMessage endianness
    msg_header->protocol = GUINT32_FROM_LE(msg_header->protocol);
    msg_header->type = GUINT32_FROM_LE(msg_header->type);
    msg_header->opaque = GUINT64_FROM_LE(msg_header->opaque);
    msg_header->size = GUINT32_FROM_LE(msg_header->size);

    monitors_config = (VDAgentMonitorsConfig *)(cmc->buffer + sizeof(*msg_header));
    if (agent_check_message(msg_header, (uint8_t *) monitors_config,
                            NULL, 0) != AGENT_CHECK_NO_ERROR) {
        goto overflow;
    }
    spice_debug("monitors_config->num_of_monitors: %d", monitors_config->num_of_monitors);
    reds_client_monitors_config(reds, monitors_config);
    spice_buffer_free(cmc);
    return;

overflow:
    spice_warning("received invalid MonitorsConfig request from client, disconnecting");
    mcc->disconnect();
    spice_buffer_free(cmc);
}