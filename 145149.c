static void sigchld_handler(int apar)
{
	int errno_save = errno;

	while (waitpid(-1, NULL, WNOHANG) > 0);
	signal(SIGCHLD, sigchld_handler);

	errno = errno_save;
}