static void reds_send_link_result(RedLinkInfo *link, uint32_t error)
{
    error = GUINT32_TO_LE(error);
    red_stream_write_all(link->stream, &error, sizeof(error));
}