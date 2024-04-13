static void reds_accept(int fd, int event, void *data)
{
    RedsState *reds = (RedsState*) data;
    int socket;

    if ((socket = accept(fd, NULL, 0)) == -1) {
        spice_warning("accept failed, %s", strerror(errno));
        return;
    }

    if (spice_server_add_client(reds, socket, 0) < 0) {
        socket_close(socket);
    }
}