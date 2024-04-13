unix_init(VALUE sock, VALUE path)
{
    return rsock_init_unixsock(sock, path, 0);
}