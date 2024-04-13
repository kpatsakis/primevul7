SPICE_GNUC_VISIBLE int spice_server_add_renderer(SpiceServer *reds, const char *name)
{
    if (!reds_add_renderer(reds, name)) {
        return -1;
    }
    return 0;
}