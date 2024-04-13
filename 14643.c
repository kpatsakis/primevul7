waitfd(int fd, int *timeoutp, short events)
{
	struct pollfd pfd;
	struct timeval t_start;
	int oerrno, r;

	pfd.fd = fd;
	pfd.events = events;
	for (; *timeoutp >= 0;) {
		monotime_tv(&t_start);
		r = poll(&pfd, 1, *timeoutp);
		oerrno = errno;
		ms_subtract_diff(&t_start, timeoutp);
		errno = oerrno;
		if (r > 0)
			return 0;
		else if (r == -1 && errno != EAGAIN && errno != EINTR)
			return -1;
		else if (r == 0)
			break;
	}
	/* timeout */
	errno = ETIMEDOUT;
	return -1;
}