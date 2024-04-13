unix_addr(VALUE sock)
{
    rb_io_t *fptr;
    struct sockaddr_un addr;
    socklen_t len = (socklen_t)sizeof addr;
    socklen_t len0 = len;

    GetOpenFile(sock, fptr);

    if (getsockname(fptr->fd, (struct sockaddr*)&addr, &len) < 0)
        rsock_sys_fail_path("getsockname(2)", fptr->pathv);
    if (len0 < len) len = len0;
    return rsock_unixaddr(&addr, len);
}