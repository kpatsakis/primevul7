static int skcipher_sendmsg_nokey(struct kiocb *unused, struct socket *sock,
				  struct msghdr *msg, size_t size)
{
	int err;

	err = skcipher_check_key(sock);
	if (err)
		return err;

	return skcipher_sendmsg(NULL, sock, msg, size);
}