static int selinux_socket_shutdown(struct socket *sock, int how)
{
	return sock_has_perm(current, sock->sk, SOCKET__SHUTDOWN);
}