static void reds_handle_new_link(RedLinkInfo *link)
{
    red_stream_set_async_error_handler(link->stream, reds_handle_link_error);
    red_stream_async_read(link->stream,
                          (uint8_t *)&link->link_header,
                          sizeof(link->link_header.magic),
                          reds_handle_read_magic_done,
                          link);
}