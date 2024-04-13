SPICE_GNUC_VISIBLE int spice_server_set_sasl(SpiceServer *s, int enabled)
{
#if HAVE_SASL
    s->config->sasl_enabled = enabled;
    return 0;
#else
    return -1;
#endif
}