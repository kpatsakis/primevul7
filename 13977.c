SPICE_GNUC_VISIBLE void spice_server_free_video_codecs(SpiceServer *reds, const char *video_codecs)
{
    g_free((char *) video_codecs);
}