waitrfd(int fd, int *timeoutp) {
	return waitfd(fd, timeoutp, POLLIN);
}