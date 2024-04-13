SPICE_GNUC_VISIBLE int spice_server_init(SpiceServer *reds, SpiceCoreInterface *core)
{
    int ret;

    ret = do_spice_init(reds, core);
    if (reds->config->renderers->len == 0) {
        reds_add_renderer(reds, default_renderer);
    }
    if (reds->config->video_codecs->len == 0) {
        reds_set_video_codecs_from_string(reds, default_video_codecs, NULL);
    }
    return ret;
}