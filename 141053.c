static int selinux_socket_getsockopt(struct socket *sock, int level,
				     int optname)
{
	return sock_has_perm(current, sock->sk, SOCKET__GETOPT);
}