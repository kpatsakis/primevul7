unixsock_connect_internal(VALUE a)
{
    struct unixsock_arg *arg = (struct unixsock_arg *)a;
    return (VALUE)rsock_connect(arg->fd, (struct sockaddr*)arg->sockaddr,
			        arg->sockaddrlen, 0);
}