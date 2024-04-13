SpiceMsgChannels *reds_msg_channels_new(RedsState *reds)
{
    SpiceMsgChannels* channels_info;

    spice_assert(reds != NULL);

    channels_info = (SpiceMsgChannels *)g_malloc(sizeof(SpiceMsgChannels)
                            + reds->channels.size() * sizeof(SpiceChannelId));

    reds_fill_channels(reds, channels_info);

    return channels_info;
}