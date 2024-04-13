static void reds_set_one_channel_security(RedsState *reds, int id, uint32_t security)
{
    ChannelSecurityOptions *security_options;

    if ((security_options = reds_find_channel_security(reds, id))) {
        security_options->options = security;
        return;
    }
    security_options = g_new(ChannelSecurityOptions, 1);
    security_options->channel_id = id;
    security_options->options = security;
    security_options->next = reds->config->channels_security;
    reds->config->channels_security = security_options;
}