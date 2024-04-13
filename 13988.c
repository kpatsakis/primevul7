SPICE_GNUC_VISIBLE int spice_server_set_exit_on_disconnect(SpiceServer *s, int flag)
{
    s->config->exit_on_disconnect = !!flag;
    return 0;
}