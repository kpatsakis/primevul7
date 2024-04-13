static int reds_security_check(RedLinkInfo *link)
{
    RedsState *reds = link->reds;
    ChannelSecurityOptions *security_option = reds_find_channel_security(reds, link->link_mess->channel_type);
    uint32_t security = security_option ? security_option->options : reds->config->default_channel_security;
    return (red_stream_is_ssl(link->stream) && (security & SPICE_CHANNEL_SECURITY_SSL)) ||
        (!red_stream_is_ssl(link->stream) && (security & SPICE_CHANNEL_SECURITY_NONE));
}