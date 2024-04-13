SPICE_GNUC_VISIBLE int spice_server_set_listen_socket_fd(SpiceServer *s, int listen_fd)
{
    s->config->spice_listen_socket_fd = listen_fd;
    return 0;
}