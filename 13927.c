static bool reds_link_mig_target_channels(RedsState *reds, RedClient *client)
{
    RedsMigTargetClient *mig_client;
    GList *item;

    spice_debug("%p", client);
    mig_client = reds_mig_target_client_find(reds, client);
    if (!mig_client) {
        spice_debug("Error: mig target client was not found");
        return FALSE;
    }

    /* Each channel should check if we are during migration, and
     * act accordingly. */
    for(item = mig_client->pending_links; item != NULL; item = item->next) {
        RedsMigPendingLink *mig_link = (RedsMigPendingLink*) item->data;
        RedChannel *channel;

        channel = reds_find_channel(reds, mig_link->link_msg->channel_type,
                                    mig_link->link_msg->channel_id);
        if (!channel) {
            spice_warning("client %p channel (%d, %d) (type, id) wasn't found",
                          client,
                          mig_link->link_msg->channel_type,
                          mig_link->link_msg->channel_id);
            continue;
        }
        reds_channel_do_link(channel, client, mig_link->link_msg, mig_link->stream);
    }

    reds_mig_target_client_free(reds, mig_client);

    return TRUE;
}