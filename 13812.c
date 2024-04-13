static void reds_handle_ssl_accept(int fd, int event, void *data)
{
    RedLinkInfo *link = (RedLinkInfo *)data;
    RedStreamSslStatus return_code = red_stream_ssl_accept(link->stream);

    switch (return_code) {
        case RED_STREAM_SSL_STATUS_ERROR:
            reds_link_free(link);
            return;
        case RED_STREAM_SSL_STATUS_WAIT_FOR_READ:
            red_watch_update_mask(link->stream->watch, SPICE_WATCH_EVENT_READ);
            return;
        case RED_STREAM_SSL_STATUS_WAIT_FOR_WRITE:
            red_watch_update_mask(link->stream->watch, SPICE_WATCH_EVENT_WRITE);
            return;
        case RED_STREAM_SSL_STATUS_OK:
            red_stream_remove_watch(link->stream);
            reds_handle_new_link(link);
    }
}