void reds_client_disconnect(RedsState *reds, RedClient *client)
{
    RedsMigTargetClient *mig_client;

    if (reds->config->exit_on_disconnect)
    {
        spice_debug("Exiting server because of client disconnect.");
        exit(0);
    }

    if (!client || client->is_disconnecting()) {
        spice_debug("client %p already during disconnection", client);
        return;
    }

    spice_debug("trace");
    /* disconnecting is set to prevent recursion because of the following:
     * main_channel_client_on_disconnect->
     *  reds_client_disconnect->red_client_destroy->main_channel...
     */
    client->set_disconnecting();

    // TODO: we need to handle agent properly for all clients!!!! (e.g., cut and paste, how?)
    // We shouldn't initialize the agent when there are still clients connected

    mig_client = reds_mig_target_client_find(reds, client);
    if (mig_client) {
        reds_mig_target_client_free(reds, mig_client);
    }

    if (reds->mig_wait_disconnect) {
        reds_mig_remove_wait_disconnect_client(reds, client);
    }

    /* note that client might be NULL, if the vdagent was once
     * up and than was removed */
    RedCharDeviceClientOpaque *client_opaque = (RedCharDeviceClientOpaque *) client;
    if (reds->agent_dev->client_exists(client_opaque)) {
        reds->agent_dev->client_remove(client_opaque);
    }

    reds->clients.remove(client);
    client->destroy();

   // TODO: we need to handle agent properly for all clients!!!! (e.g., cut and paste, how? Maybe throw away messages
   // if we are in the middle of one from another client)
    if (reds->clients.empty()) {
        /* Let the agent know the client is disconnected */
        if (reds->agent_dev->priv->agent_attached) {
            RedCharDeviceWriteBuffer *char_dev_buf =
                vdagent_new_write_buffer(reds->agent_dev.get(),
                                         VD_AGENT_CLIENT_DISCONNECTED,
                                         0,
                                         false);

            reds->agent_dev->write_buffer_add(char_dev_buf);
        }

        /* Reset write filter to start with clean state on client reconnect */
        agent_msg_filter_init(&reds->agent_dev->priv->write_filter, reds->config->agent_copypaste,
                              reds->config->agent_file_xfer,
                              reds_use_client_monitors_config(reds), TRUE);

        /* Throw away pending chunks from the current (if any) and future
         *  messages read from the agent */
        reds->agent_dev->priv->read_filter.result = AGENT_MSG_FILTER_DISCARD;
        reds->agent_dev->priv->read_filter.discard_all = TRUE;
        g_free(reds->agent_dev->priv->mig_data);
        reds->agent_dev->priv->mig_data = NULL;

        reds_mig_cleanup(reds);
    }
}