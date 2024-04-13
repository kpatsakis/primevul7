daemonized(void)
{
	int fd;

	if ((fd = open(_PATH_TTY, O_RDONLY | O_NOCTTY)) >= 0) {
		close(fd);
		return 0;	/* have controlling terminal */
	}
	if (getppid() != 1)
		return 0;	/* parent is not init */
	if (getsid(0) != getpid())
		return 0;	/* not session leader */
	debug3("already daemonized");
	return 1;
}