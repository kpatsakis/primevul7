static int skcipher_recvmsg_nokey(struct kiocb *unused, struct socket *sock,
				  struct msghdr *msg, size_t ignored, int flags)
{
	int err;

	err = skcipher_check_key(sock);
	if (err)
		return err;

	return skcipher_recvmsg(NULL, sock, msg, ignored, flags);
}