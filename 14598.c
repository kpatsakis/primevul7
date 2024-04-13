timeout_connect(int sockfd, const struct sockaddr *serv_addr,
    socklen_t addrlen, int *timeoutp)
{
	int optval = 0;
	socklen_t optlen = sizeof(optval);

	/* No timeout: just do a blocking connect() */
	if (timeoutp == NULL || *timeoutp <= 0)
		return connect(sockfd, serv_addr, addrlen);

	set_nonblock(sockfd);
	for (;;) {
		if (connect(sockfd, serv_addr, addrlen) == 0) {
			/* Succeeded already? */
			unset_nonblock(sockfd);
			return 0;
		} else if (errno == EINTR)
			continue;
		else if (errno != EINPROGRESS)
			return -1;
		break;
	}

	if (waitfd(sockfd, timeoutp, POLLIN | POLLOUT) == -1)
		return -1;

	/* Completed or failed */
	if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) == -1) {
		debug("getsockopt: %s", strerror(errno));
		return -1;
	}
	if (optval != 0) {
		errno = optval;
		return -1;
	}
	unset_nonblock(sockfd);
	return 0;
}