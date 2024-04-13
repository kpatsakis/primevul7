static void reds_set_video_codecs(RedsState *reds, GArray *video_codecs)
{
    /* The video_codecs array is immutable */
    g_clear_pointer(&reds->config->video_codecs, g_array_unref);

    spice_return_if_fail(video_codecs != NULL);

    reds->config->video_codecs = video_codecs;
}