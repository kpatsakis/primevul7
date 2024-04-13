SPICE_GNUC_VISIBLE void spice_server_set_name(SpiceServer *s, const char *name)
{
    g_free(s->config->spice_name);
    s->config->spice_name = g_strdup(name);
}