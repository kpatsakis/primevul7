static void reds_handle_main_link(RedsState *reds, RedLinkInfo *link)
{
    RedClient *client;
    RedStream *stream;
    SpiceLinkMess *link_mess;
    uint32_t connection_id;
    MainChannelClient *mcc;
    int mig_target = FALSE;
    RedChannelCapabilities caps;

    spice_debug("trace");
    spice_assert(reds->main_channel);

    reds_late_initialization(reds);

    link_mess = link->link_mess;
    if (!reds->allow_multiple_clients) {
        reds_disconnect(reds);
    }

    if (link_mess->connection_id == 0) {
        reds_send_link_result(link, SPICE_LINK_ERR_OK);
        while((connection_id = rand()) == 0);
        mig_target = FALSE;
    } else {
        // TODO: make sure link_mess->connection_id is the same
        // connection id the migration src had (use vmstate to store the connection id)
        reds_send_link_result(link, SPICE_LINK_ERR_OK);
        connection_id = link_mess->connection_id;
        mig_target = TRUE;
    }

    reds->mig_inprogress = FALSE;
    reds->mig_wait_connect = FALSE;
    reds->mig_wait_disconnect = FALSE;

    reds_info_new_channel(link, connection_id);
    stream = link->stream;
    link->stream = NULL;
    client = red_client_new(reds, mig_target);
    reds->clients.push_front(client);

    red_channel_capabilities_init_from_link_message(&caps, link_mess);
    mcc = main_channel_link(reds->main_channel.get(), client,
                            stream, connection_id, mig_target,
                            &caps);
    red_channel_capabilities_reset(&caps);
    spice_debug("NEW Client %p mcc %p connect-id %d", client, mcc, connection_id);

    if (reds->vdagent) {
        if (mig_target) {
            spice_warning("unexpected: vdagent attached to destination during migration");
        }
        agent_msg_filter_config(&reds->agent_dev->priv->read_filter,
                                reds->config->agent_copypaste,
                                reds->config->agent_file_xfer,
                                reds_use_client_monitors_config(reds));
        reds->agent_dev->priv->read_filter.discard_all = FALSE;
        reds->agent_dev->priv->plug_generation++;
    }

    if (!mig_target) {
        mcc->push_init(reds->qxl_instances.size(), reds->mouse_mode,
                       reds->is_client_mouse_allowed,
                       reds_get_mm_time() - MM_TIME_DELTA,
                       reds_qxl_ram_size(reds));
        if (reds->config->spice_name)
            mcc->push_name(reds->config->spice_name);
        if (reds->config->spice_uuid_is_set)
            mcc->push_uuid(reds->config->spice_uuid);
    } else {
        reds_mig_target_client_add(reds, client);
    }

    if (red_stream_get_family(stream) != AF_UNIX) {
        mcc->start_net_test(!mig_target);
    }
}