privsep_init(void)
{
	int pipefds[2];
	pid_t pid;

	if (privsep_enabled())
		return 0;

	if (pipe(pipefds) != 0) {
		flog(LOG_ERR, "Couldn't create privsep pipe.");
		return (-1);
	}

	pid = fork();
	if (pid == -1) {
		flog(LOG_ERR, "Couldn't fork for privsep.");
		return (-1);
	}

	if (pid == 0) {
		int nullfd;

		/* This will be the privileged child */
		close(pipefds[1]);
		pfd = pipefds[0];

		/* Detach from stdio */
		nullfd = open("/dev/null", O_RDONLY);
		if (nullfd < 0) {
			perror("/dev/null");
			close(pfd);
			_exit(1);
		}
		dup2(nullfd, 0);
		dup2(nullfd, 1);
		/* XXX: we'll keep stderr open in debug mode for better logging */
		if (get_debuglevel() == 0)
			dup2(nullfd, 2);

		privsep_read_loop();
		close(pfd);
		flog(LOG_ERR, "Exiting, privsep_read_loop is complete.\n");
		_exit(0);
	}

	/* Continue execution (will drop privileges soon) */
	close(pipefds[0]);
	pfd = pipefds[1];

	return 0;
}