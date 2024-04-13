SPICE_GNUC_VISIBLE const char *spice_server_get_video_codecs(SpiceServer *reds)
{
    return video_codecs_to_string(reds_get_video_codecs(reds), ";");
}