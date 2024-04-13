static void reds_fill_channels(RedsState *reds, SpiceMsgChannels *channels_info)
{
    int used_channels = 0;

    for (const auto channel: reds->channels) {
        if (reds->clients.size() > 1 &&
            !channel_supports_multiple_clients(channel.get())) {
            continue;
        }
        channels_info->channels[used_channels].type = channel->type();
        channels_info->channels[used_channels].id = channel->id();
        used_channels++;
    }

    channels_info->num_of_channels = used_channels;
    if (used_channels != reds->channels.size()) {
        spice_warning("sent %d out of %zd", used_channels, reds->channels.size());
    }
}