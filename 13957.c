static int reds_main_channel_connected(RedsState *reds)
{
    return reds->main_channel && reds->main_channel->is_connected();
}