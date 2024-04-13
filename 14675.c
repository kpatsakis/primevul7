get_rdomain(int fd)
{
#if defined(HAVE_SYS_GET_RDOMAIN)
	return sys_get_rdomain(fd);
#elif defined(__OpenBSD__)
	int rtable;
	char *ret;
	socklen_t len = sizeof(rtable);

	if (getsockopt(fd, SOL_SOCKET, SO_RTABLE, &rtable, &len) == -1) {
		error("Failed to get routing domain for fd %d: %s",
		    fd, strerror(errno));
		return NULL;
	}
	xasprintf(&ret, "%d", rtable);
	return ret;
#else /* defined(__OpenBSD__) */
	return NULL;
#endif
}