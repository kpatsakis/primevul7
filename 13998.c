SPICE_GNUC_VISIBLE int spice_server_set_port(SpiceServer *reds, int port)
{
    if (port < 0 || port > 0xffff) {
        return -1;
    }
    reds->config->spice_port = port;
    return 0;
}