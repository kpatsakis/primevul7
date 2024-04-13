static void reds_mig_target_client_add_pending_link(RedsMigTargetClient *client,
                                                    SpiceLinkMess *link_msg,
                                                    RedStream *stream)
{
    RedsMigPendingLink *mig_link;

    spice_assert(client);
    mig_link = g_new0(RedsMigPendingLink, 1);
    mig_link->link_msg = link_msg;
    mig_link->stream = stream;

    client->pending_links = g_list_append(client->pending_links, mig_link);
}