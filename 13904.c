static void reds_cleanup_net(SpiceServer *reds)
{
    if (reds->listen_socket != -1) {
       red_watch_remove(reds->listen_watch);
       if (reds->config->spice_listen_socket_fd != reds->listen_socket) {
          socket_close(reds->listen_socket);
       }
       reds->listen_watch = NULL;
       reds->listen_socket = -1;
    }
    if (reds->secure_listen_socket != -1) {
       red_watch_remove(reds->secure_listen_watch);
       socket_close(reds->secure_listen_socket);
       reds->secure_listen_watch = NULL;
       reds->secure_listen_socket = -1;
    }
}