char *reds_get_video_codec_fullname(RedVideoCodec *codec)
{
    int i;
    const char *encoder_name = NULL;
    const char *codec_name = get_index_name(video_codec_names, codec->type);

    spice_assert(codec_name);

    for (i = 0; i < G_N_ELEMENTS(video_encoder_procs); i++) {
        if (video_encoder_procs[i] == codec->create) {
            encoder_name = get_index_name(video_encoder_names, i);
            break;
        }
    }
    spice_assert(encoder_name);

    return g_strdup_printf("%s:%s", encoder_name, codec_name);
}