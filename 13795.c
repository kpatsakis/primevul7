SPICE_GNUC_VISIBLE int spice_server_set_video_codecs(SpiceServer *reds, const char *video_codecs)
{
    unsigned int installed = 0;

    reds_set_video_codecs_from_string(reds, video_codecs, &installed);

    if (!installed) {
        return -1;
    }
    reds_on_vc_change(reds);

    return 0;
}