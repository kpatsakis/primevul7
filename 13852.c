SPICE_GNUC_VISIBLE int spice_server_set_streaming_video(SpiceServer *reds, int value)
{
    if (value != SPICE_STREAM_VIDEO_OFF &&
        value != SPICE_STREAM_VIDEO_ALL &&
        value != SPICE_STREAM_VIDEO_FILTER)
        return -1;
    reds->config->streaming_video = value;
    reds_on_sv_change(reds);
    return 0;
}