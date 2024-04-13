stdfd_devnull(int do_stdin, int do_stdout, int do_stderr)
{
	int devnull, ret = 0;

	if ((devnull = open(_PATH_DEVNULL, O_RDWR)) == -1) {
		error_f("open %s: %s", _PATH_DEVNULL,
		    strerror(errno));
		return -1;
	}
	if ((do_stdin && dup2(devnull, STDIN_FILENO) == -1) ||
	    (do_stdout && dup2(devnull, STDOUT_FILENO) == -1) ||
	    (do_stderr && dup2(devnull, STDERR_FILENO) == -1)) {
		error_f("dup2: %s", strerror(errno));
		ret = -1;
	}
	if (devnull > STDERR_FILENO)
		close(devnull);
	return ret;
}