static RedLinkInfo *reds_init_client_ssl_connection(RedsState *reds, int socket)
{
    RedLinkInfo *link;
    RedStreamSslStatus ssl_status;

    link = reds_init_client_connection(reds, socket);
    if (link == NULL) {
        return NULL;
    }

    ssl_status = red_stream_enable_ssl(link->stream, reds->ctx);
    switch (ssl_status) {
        case RED_STREAM_SSL_STATUS_OK:
            reds_handle_new_link(link);
            return link;
        case RED_STREAM_SSL_STATUS_ERROR:
            goto error;
        case RED_STREAM_SSL_STATUS_WAIT_FOR_READ:
            link->stream->watch = reds_core_watch_add(reds, link->stream->socket,
                                                      SPICE_WATCH_EVENT_READ,
                                                      reds_handle_ssl_accept, link);
            break;
        case RED_STREAM_SSL_STATUS_WAIT_FOR_WRITE:
            link->stream->watch = reds_core_watch_add(reds, link->stream->socket,
                                                      SPICE_WATCH_EVENT_WRITE,
                                                      reds_handle_ssl_accept, link);
            break;
    }
    return link;

error:
    /* close the stream but do not close the socket, this API is
     * supposed to not close it if it fails */
    link->stream->socket = -1;
    reds_link_free(link);
    return NULL;
}