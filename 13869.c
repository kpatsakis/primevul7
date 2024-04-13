SPICE_GNUC_VISIBLE int spice_server_set_channel_security(SpiceServer *s, const char *channel, int security)
{
    int type;
    if (channel == NULL) {
        s->config->default_channel_security = security;
        return 0;
    }
    type = red_channel_name_to_type(channel);
#ifndef USE_SMARTCARD
    if (type == SPICE_CHANNEL_SMARTCARD) {
        type = -1;
    }
#endif
    if (type == -1) {
        return -1;
    }

    reds_set_one_channel_security(s, type, security);
    return 0;
}