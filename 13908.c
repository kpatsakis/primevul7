SPICE_GNUC_VISIBLE SpiceServer *spice_server_new(void)
{
    const char *record_filename;
    RedsState *reds = new RedsState;

    reds->config = g_new0(RedServerConfig, 1);
    reds->config->default_channel_security =
        SPICE_CHANNEL_SECURITY_NONE | SPICE_CHANNEL_SECURITY_SSL;
    reds->config->renderers = g_array_sized_new(FALSE, TRUE, sizeof(uint32_t), RED_RENDERER_LAST);
    reds->config->spice_port = -1;
    reds->config->spice_secure_port = -1;
    reds->config->spice_listen_socket_fd = -1;
    reds->config->spice_family = PF_UNSPEC;
    reds->config->sasl_enabled = 0; // sasl disabled by default
#if HAVE_SASL
    reds->config->sasl_appname = NULL; // default to "spice" if NULL
#endif
    reds->config->spice_uuid_is_set = FALSE;
    memset(reds->config->spice_uuid, 0, sizeof(reds->config->spice_uuid));
    reds->config->ticketing_enabled = TRUE; /* ticketing enabled by default */
    reds->config->streaming_video = SPICE_STREAM_VIDEO_FILTER;
    reds->config->video_codecs = g_array_new(FALSE, FALSE, sizeof(RedVideoCodec));
    reds->config->image_compression = SPICE_IMAGE_COMPRESSION_AUTO_GLZ;
    reds->config->playback_compression = TRUE;
    reds->config->jpeg_state = SPICE_WAN_COMPRESSION_AUTO;
    reds->config->zlib_glz_state = SPICE_WAN_COMPRESSION_AUTO;
    reds->config->agent_mouse = TRUE;
    reds->config->agent_copypaste = TRUE;
    reds->config->agent_file_xfer = TRUE;
    reds->config->exit_on_disconnect = FALSE;
#ifdef RED_STATISTICS
    reds->stat_file = stat_file_new(REDS_MAX_STAT_NODES);
    /* Create an initial node. This will be the 0 node making easier
     * to initialize node references.
     */
    stat_file_add_node(reds->stat_file, INVALID_STAT_REF, "default_channel", TRUE);
#endif
    reds->listen_socket = -1;
    reds->secure_listen_socket = -1;

    /* This environment was in red-worker so the "WORKER" in it.
     * For compatibility reason we maintain the old name */
    record_filename = getenv("SPICE_WORKER_RECORD_FILENAME");
    if (record_filename) {
        reds->record = red_record_new(record_filename);
    }
    return reds;
}