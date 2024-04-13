static void reds_handle_link(RedLinkInfo *link)
{
    RedsState *reds = link->reds;

    red_stream_remove_watch(link->stream);
    if (link->link_mess->channel_type == SPICE_CHANNEL_MAIN) {
        reds_handle_main_link(reds, link);
    } else {
        reds_handle_other_links(reds, link);
    }
    reds_link_free(link);
}