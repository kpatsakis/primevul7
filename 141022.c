static int selinux_socket_getsockname(struct socket *sock)
{
	return sock_has_perm(current, sock->sk, SOCKET__GETATTR);
}