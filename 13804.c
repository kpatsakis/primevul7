static void reds_handle_auth_mechanism(void *opaque)
{
    RedLinkInfo *link = (RedLinkInfo *)opaque;
    RedsState *reds = link->reds;

    spice_debug("Auth method: %d", link->auth_mechanism.auth_mechanism);

    link->auth_mechanism.auth_mechanism = GUINT32_FROM_LE(link->auth_mechanism.auth_mechanism);
    if (link->auth_mechanism.auth_mechanism == SPICE_COMMON_CAP_AUTH_SPICE
        && !reds->config->sasl_enabled
        ) {
        reds_get_spice_ticket(link);
#if HAVE_SASL
    } else if (link->auth_mechanism.auth_mechanism == SPICE_COMMON_CAP_AUTH_SASL) {
        spice_debug("Starting SASL");
        reds_start_auth_sasl(link);
#endif
    } else {
        spice_warning("Unknown auth method, disconnecting");
        if (reds->config->sasl_enabled) {
            spice_warning("Your client doesn't handle SASL?");
        }
        reds_send_link_error(link, SPICE_LINK_ERR_INVALID_DATA);
        reds_link_free(link);
    }
}