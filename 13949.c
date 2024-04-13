static void reds_channel_init_auth_caps(RedLinkInfo *link, RedChannel *channel)
{
    RedsState *reds = link->reds;
    if (reds->config->sasl_enabled && !link->skip_auth) {
        channel->set_common_cap(SPICE_COMMON_CAP_AUTH_SASL);
    } else {
        channel->set_common_cap(SPICE_COMMON_CAP_AUTH_SPICE);
    }
}