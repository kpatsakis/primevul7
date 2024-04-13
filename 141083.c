static int selinux_socket_recvmsg(struct socket *sock, struct msghdr *msg,
				  int size, int flags)
{
	return sock_has_perm(current, sock->sk, SOCKET__READ);
}