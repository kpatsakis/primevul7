static bool reds_send_link_error(RedLinkInfo *link, uint32_t error)
{
    struct {
        SpiceLinkHeader header;
        SpiceLinkReply reply;
    } msg;
    SPICE_VERIFY(sizeof(msg) == sizeof(SpiceLinkHeader) + sizeof(SpiceLinkReply));

    msg.header.magic = SPICE_MAGIC;
    msg.header.size = GUINT32_TO_LE(sizeof(msg.reply));
    msg.header.major_version = GUINT32_TO_LE(SPICE_VERSION_MAJOR);
    msg.header.minor_version = GUINT32_TO_LE(SPICE_VERSION_MINOR);
    memset(&msg.reply, 0, sizeof(msg.reply));
    msg.reply.error = GUINT32_TO_LE(error);
    return red_stream_write_all(link->stream, &msg, sizeof(msg));
}