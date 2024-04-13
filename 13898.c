static void reds_handle_read_link_done(void *opaque)
{
    RedLinkInfo *link = (RedLinkInfo *)opaque;
    RedsState *reds = link->reds;
    SpiceLinkMess *link_mess = link->link_mess;
    uint32_t num_caps;
    uint32_t *caps;
    int auth_selection;
    unsigned int i;

    link_mess->caps_offset = GUINT32_FROM_LE(link_mess->caps_offset);
    link_mess->connection_id = GUINT32_FROM_LE(link_mess->connection_id);
    link_mess->num_channel_caps = GUINT32_FROM_LE(link_mess->num_channel_caps);
    link_mess->num_common_caps = GUINT32_FROM_LE(link_mess->num_common_caps);

    /* Prevent DoS. Currently we defined only 13 capabilities,
     * I expect 1024 to be valid for quite a lot time */
    if (link_mess->num_channel_caps > 1024 || link_mess->num_common_caps > 1024) {
        reds_send_link_error(link, SPICE_LINK_ERR_INVALID_DATA);
        reds_link_free(link);
        return;
    }

    num_caps = link_mess->num_common_caps + link_mess->num_channel_caps;
    caps = (uint32_t *)((uint8_t *)link_mess + link_mess->caps_offset);

    if (num_caps && (num_caps * sizeof(uint32_t) + link_mess->caps_offset >
                     link->link_header.size ||
                     link_mess->caps_offset < sizeof(*link_mess))) {
        reds_send_link_error(link, SPICE_LINK_ERR_INVALID_DATA);
        reds_link_free(link);
        return;
    }

    for(i = 0; i < num_caps;i++)
        caps[i] = GUINT32_FROM_LE(caps[i]);

    auth_selection = red_link_info_test_capability(link,
                                                   SPICE_COMMON_CAP_PROTOCOL_AUTH_SELECTION);

    if (!reds_security_check(link)) {
        if (red_stream_is_ssl(link->stream)) {
            spice_warning("spice channels %d should not be encrypted", link_mess->channel_type);
            reds_send_link_error(link, SPICE_LINK_ERR_NEED_UNSECURED);
        } else {
            spice_warning("spice channels %d should be encrypted", link_mess->channel_type);
            reds_send_link_error(link, SPICE_LINK_ERR_NEED_SECURED);
        }
        reds_link_free(link);
        return;
    }

    if (!reds_send_link_ack(reds, link)) {
        reds_link_free(link);
        return;
    }

    if (!auth_selection) {
        if (reds->config->sasl_enabled && !link->skip_auth) {
            spice_warning("SASL enabled, but peer supports only spice authentication");
            reds_send_link_error(link, SPICE_LINK_ERR_VERSION_MISMATCH);
            return;
        }
        spice_warning("Peer doesn't support AUTH selection");
        reds_get_spice_ticket(link);
    } else {
        red_stream_async_read(link->stream,
                              (uint8_t *)&link->auth_mechanism,
                              sizeof(SpiceLinkAuthMechanism),
                              reds_handle_auth_mechanism,
                              link);
    }
}