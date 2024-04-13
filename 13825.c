static void reds_info_new_channel(RedLinkInfo *link, int connection_id)
{
    spice_debug("channel %d:%d, connected successfully, over %s link",
                link->link_mess->channel_type,
                link->link_mess->channel_id,
                red_stream_is_ssl(link->stream) ? "Secure" : "Non Secure");
    /* add info + send event */
    red_stream_set_channel(link->stream, connection_id,
                           link->link_mess->channel_type,
                           link->link_mess->channel_id);
    red_stream_push_channel_event(link->stream, SPICE_CHANNEL_EVENT_INITIALIZED);
}