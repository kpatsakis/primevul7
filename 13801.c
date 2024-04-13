static int reds_init_net(RedsState *reds)
{
    if (reds->config->spice_port != -1 || reds->config->spice_family == AF_UNIX) {
        reds->listen_socket = reds_init_socket(reds->config->spice_addr, reds->config->spice_port, reds->config->spice_family);
        if (-1 == reds->listen_socket) {
            return -1;
        }
        reds->listen_watch = reds_core_watch_add(reds, reds->listen_socket,
                                                 SPICE_WATCH_EVENT_READ,
                                                 reds_accept, reds);
        if (reds->listen_watch == NULL) {
            return -1;
        }
    }

    if (reds->config->spice_secure_port != -1) {
        reds->secure_listen_socket = reds_init_socket(reds->config->spice_addr, reds->config->spice_secure_port,
                                                      reds->config->spice_family);
        if (-1 == reds->secure_listen_socket) {
            return -1;
        }
        reds->secure_listen_watch = reds_core_watch_add(reds, reds->secure_listen_socket,
                                                        SPICE_WATCH_EVENT_READ,
                                                        reds_accept_ssl_connection, reds);
        if (reds->secure_listen_watch == NULL) {
            return -1;
        }
    }

    if (reds->config->spice_listen_socket_fd != -1 ) {
        reds->listen_socket = reds->config->spice_listen_socket_fd;
        reds->listen_watch = reds_core_watch_add(reds, reds->listen_socket,
                                                 SPICE_WATCH_EVENT_READ,
                                                 reds_accept, reds);
        if (reds->listen_watch == NULL) {
            return -1;
        }
    }
    return 0;
}