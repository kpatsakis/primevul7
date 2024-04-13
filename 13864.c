static int reds_set_video_codecs_from_string(RedsState *reds, const char *codecs,
                                             unsigned int *installed)
{
    char *encoder_name, *codec_name;
    GArray *video_codecs;
    int invalid_codecs = 0;

    g_return_val_if_fail(codecs != NULL, -1);

    if (strcmp(codecs, "auto") == 0) {
        codecs = default_video_codecs;
    }

    video_codecs = g_array_new(FALSE, FALSE, sizeof(RedVideoCodec));
    char *codecs_copy = g_strdup_printf("%s;", codecs);
    char *c = codecs_copy;
    while ( (c = parse_next_video_codec(c, &encoder_name, &codec_name)) ) {
        uint32_t encoder_index, codec_index;
        if (!encoder_name || !codec_name) {
            spice_warning("spice: invalid encoder:codec value at %s", codecs);
            invalid_codecs++;

        } else if (!get_name_index(video_encoder_names, encoder_name, &encoder_index)){
            spice_warning("spice: unknown video encoder %s", encoder_name);
            invalid_codecs++;

        } else if (!get_name_index(video_codec_names, codec_name, &codec_index)) {
            spice_warning("spice: unknown video codec %s", codec_name);
            invalid_codecs++;

        } else if (!video_encoder_procs[encoder_index]) {
            spice_warning("spice: unsupported video encoder %s", encoder_name);
            invalid_codecs++;

        } else {
            RedVideoCodec new_codec;
            new_codec.create = video_encoder_procs[encoder_index];
            new_codec.type = (SpiceVideoCodecType) video_codec_names[codec_index].id;
            new_codec.cap = video_codec_caps[codec_index];
            g_array_append_val(video_codecs, new_codec);
        }

        codecs = c;
    }

    if (installed) {
        *installed = video_codecs->len;
    }

    if (video_codecs->len == 0) {
        spice_warning("Failed to set video codecs, input string: '%s'", codecs);
        g_array_unref(video_codecs);
    } else {
        reds_set_video_codecs(reds, video_codecs);
    }

    g_free(codecs_copy);

    return invalid_codecs;
}