static bool reds_send_link_ack(RedsState *reds, RedLinkInfo *link)
{
    struct {
        SpiceLinkHeader header;
        SpiceLinkReply ack;
    } msg;
    RedChannel *channel;
    const RedChannelCapabilities *channel_caps;
    BUF_MEM *bmBuf;
    BIO *bio = NULL;
    int ret = FALSE;
    size_t hdr_size;

    SPICE_VERIFY(sizeof(msg) == sizeof(SpiceLinkHeader) + sizeof(SpiceLinkReply));

    msg.header.magic = SPICE_MAGIC;
    hdr_size = sizeof(msg.ack);
    msg.header.major_version = GUINT32_TO_LE(SPICE_VERSION_MAJOR);
    msg.header.minor_version = GUINT32_TO_LE(SPICE_VERSION_MINOR);

    msg.ack.error = GUINT32_TO_LE(SPICE_LINK_ERR_OK);

    channel = reds_find_channel(reds, link->link_mess->channel_type,
                                link->link_mess->channel_id);
    if (!channel) {
        if (link->link_mess->channel_type != SPICE_CHANNEL_MAIN) {
            spice_warning("Received wrong header: channel_type != SPICE_CHANNEL_MAIN");
            return FALSE;
        }
        spice_assert(reds->main_channel);
        channel = reds->main_channel.get();
    }

    reds_channel_init_auth_caps(link, channel); /* make sure common caps are set */

    channel_caps = channel->get_local_capabilities();
    msg.ack.num_common_caps = GUINT32_TO_LE(channel_caps->num_common_caps);
    msg.ack.num_channel_caps = GUINT32_TO_LE(channel_caps->num_caps);
    hdr_size += channel_caps->num_common_caps * sizeof(uint32_t);
    hdr_size += channel_caps->num_caps * sizeof(uint32_t);
    msg.header.size = GUINT32_TO_LE(hdr_size);
    msg.ack.caps_offset = GUINT32_TO_LE(sizeof(SpiceLinkReply));
    if (!reds->config->sasl_enabled
        || !red_link_info_test_capability(link, SPICE_COMMON_CAP_AUTH_SASL)) {
        if (!(link->tiTicketing.rsa = RSA_new())) {
            spice_warning("RSA new failed");
            red_dump_openssl_errors();
            return FALSE;
        }

        if (!(bio = BIO_new(BIO_s_mem()))) {
            spice_warning("BIO new failed");
            red_dump_openssl_errors();
            return FALSE;
        }

        if (RSA_generate_key_ex(link->tiTicketing.rsa,
                                SPICE_TICKET_KEY_PAIR_LENGTH,
                                link->tiTicketing.bn,
                                NULL) != 1) {
            spice_warning("Failed to generate %d bits RSA key",
                          SPICE_TICKET_KEY_PAIR_LENGTH);
            red_dump_openssl_errors();
            goto end;
        }
        link->tiTicketing.rsa_size = RSA_size(link->tiTicketing.rsa);

        i2d_RSA_PUBKEY_bio(bio, link->tiTicketing.rsa);
        BIO_get_mem_ptr(bio, &bmBuf);
        memcpy(msg.ack.pub_key, bmBuf->data, sizeof(msg.ack.pub_key));
    } else {
        /* if the client sets the AUTH_SASL cap, it indicates that it
         * supports SASL, and will use it if the server supports SASL as
         * well. Moreover, a client setting the AUTH_SASL cap also
         * indicates that it will not try using the RSA-related content
         * in the SpiceLinkReply message, so we don't need to initialize
         * it. Reason to avoid this is to fix auth in fips mode where
         * the generation of a 1024 bit RSA key as we are trying to do
         * will fail.
         */
        spice_warning("not initialising RSA key");
        memset(msg.ack.pub_key, '\0', sizeof(msg.ack.pub_key));
    }

    if (!red_stream_write_all(link->stream, &msg, sizeof(msg)))
        goto end;
    for (unsigned int i = 0; i < channel_caps->num_common_caps; i++) {
        guint32 cap = GUINT32_TO_LE(channel_caps->common_caps[i]);
        if (!red_stream_write_all(link->stream, &cap, sizeof(cap)))
            goto end;
    }
    for (unsigned int i = 0; i < channel_caps->num_caps; i++) {
        guint32 cap = GUINT32_TO_LE(channel_caps->caps[i]);
        if (!red_stream_write_all(link->stream, &cap, sizeof(cap)))
            goto end;
    }

    ret = TRUE;

end:
    if (bio != NULL)
        BIO_free(bio);
    return ret;
}