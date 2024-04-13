static void reds_channel_do_link(RedChannel *channel, RedClient *client,
                                 SpiceLinkMess *link_msg,
                                 RedStream *stream)
{
    RedChannelCapabilities caps;

    spice_assert(channel);
    spice_assert(link_msg);
    spice_assert(stream);

    red_channel_capabilities_init_from_link_message(&caps, link_msg);
    channel->connect(client, stream,
                     client->during_migrate_at_target(), &caps);
    red_channel_capabilities_reset(&caps);
}