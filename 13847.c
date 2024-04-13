static RedLinkInfo *reds_init_client_connection(RedsState *reds, int socket)
{
    RedLinkInfo *link;

    if (!red_socket_set_non_blocking(socket, TRUE)) {
        return NULL;
    }

    if (!red_socket_set_no_delay(socket, TRUE)) {
        return NULL;
    }

    red_socket_set_keepalive(socket, TRUE, KEEPALIVE_TIMEOUT);
    red_socket_set_nosigpipe(socket, true);

    link = g_new0(RedLinkInfo, 1);
    link->reds = reds;
    link->stream = red_stream_new(reds, socket);

    /* gather info + send event */

    red_stream_push_channel_event(link->stream, SPICE_CHANNEL_EVENT_CONNECTED);

    openssl_init(link);

    return link;
}