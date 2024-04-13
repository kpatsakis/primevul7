static void reds_accept_ssl_connection(int fd, int event, void *data)
{
    RedsState *reds = (RedsState*) data;
    RedLinkInfo *link;
    int socket;

    if ((socket = accept(fd, NULL, 0)) == -1) {
        spice_warning("accept failed, %s", strerror(errno));
        return;
    }

    if (!(link = reds_init_client_ssl_connection(reds, socket))) {
        socket_close(socket);
    }
}