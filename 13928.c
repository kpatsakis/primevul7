static void reds_config_free(RedServerConfig *config)
{
    ChannelSecurityOptions *curr, *next;

    reds_mig_release(config);
    for (curr = config->channels_security; curr; curr = next) {
        next = curr->next;
        g_free(curr);
    }
#if HAVE_SASL
    g_free(config->sasl_appname);
#endif
    g_free(config->spice_name);
    g_array_unref(config->renderers);
    g_array_unref(config->video_codecs);
    g_free(config);
}