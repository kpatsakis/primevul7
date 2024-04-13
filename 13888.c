SPICE_GNUC_VISIBLE int spice_server_set_playback_compression(SpiceServer *reds, int enable)
{
    reds->config->playback_compression = !!enable;
    snd_set_playback_compression(enable);
    return 0;
}