SPICE_GNUC_VISIBLE int spice_server_set_sasl_appname(SpiceServer *s, const char *appname)
{
#if HAVE_SASL
    g_free(s->config->sasl_appname);
    s->config->sasl_appname = g_strdup(appname);
    return 0;
#else
    return -1;
#endif
}