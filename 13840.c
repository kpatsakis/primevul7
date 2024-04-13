static ChannelSecurityOptions *reds_find_channel_security(RedsState *reds, int id)
{
    ChannelSecurityOptions *now = reds->config->channels_security;
    while (now && now->channel_id != id) {
        now = now->next;
    }
    return now;
}