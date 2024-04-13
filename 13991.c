static void reds_handle_read_header_done(void *opaque)
{
    RedLinkInfo *link = (RedLinkInfo *)opaque;
    SpiceLinkHeader *header = &link->link_header;

    header->major_version = GUINT32_FROM_LE(header->major_version);
    header->minor_version = GUINT32_FROM_LE(header->minor_version);
    header->size = GUINT32_FROM_LE(header->size);

    if (header->major_version != SPICE_VERSION_MAJOR) {
        if (header->major_version > 0) {
            reds_send_link_error(link, SPICE_LINK_ERR_VERSION_MISMATCH);
        }

        spice_warning("version mismatch");
        reds_link_free(link);
        return;
    }

    /* the check for 4096 is to avoid clients to cause arbitrary big memory allocations */
    if (header->size < sizeof(SpiceLinkMess) || header->size > 4096) {
        reds_send_link_error(link, SPICE_LINK_ERR_INVALID_DATA);
        spice_warning("bad size %u", header->size);
        reds_link_free(link);
        return;
    }

    link->link_mess = (SpiceLinkMess*) g_malloc(header->size);

    red_stream_async_read(link->stream,
                          (uint8_t *)link->link_mess,
                          header->size,
                          reds_handle_read_link_done,
                          link);
}