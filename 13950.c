static void reds_handle_read_magic_done(void *opaque)
{
    RedLinkInfo *link = (RedLinkInfo *)opaque;
    const SpiceLinkHeader *header = &link->link_header;

    if (header->magic != SPICE_MAGIC) {
        /* Attempt to detect and support a WebSocket connection,
           which will be proceeded by a variable length GET style request.
           We cannot know we are dealing with a WebSocket connection
           until we have read at least 3 bytes, and we will have to
           read many more bytes than are contained in a SpiceLinkHeader.
           So we may as well read a SpiceLinkHeader's worth of data, and if it's
           clear that a WebSocket connection was requested, we switch
           before proceeding further. */
        if (red_stream_is_websocket(link->stream, &header->magic, sizeof(header->magic))) {
            reds_handle_new_link(link);
            return;
        }
        reds_send_link_error(link, SPICE_LINK_ERR_INVALID_MAGIC);
        reds_link_free(link);
        return;
    }

    red_stream_async_read(link->stream,
                          ((uint8_t *)&link->link_header) + sizeof(header->magic),
                          sizeof(SpiceLinkHeader) - sizeof(header->magic),
                          reds_handle_read_header_done,
                          link);
}