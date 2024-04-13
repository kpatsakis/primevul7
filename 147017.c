static void sigchld_reaper(void) {
	WAIT_T waiter;
	PID_T pid;

	do {
		pid = waitpid(-1, &waiter, WNOHANG);
		switch (pid) {
		case -1:
			if (errno == EINTR)
				continue;
			Debug(DPROC, ("[%ld] sigchld...no children\n", (long) getpid()))
				break;
		case 0:
			Debug(DPROC, ("[%ld] sigchld...no dead kids\n", (long) getpid()))
				break;
		default:
			Debug(DPROC,
				("[%ld] sigchld...pid #%ld died, stat=%d\n",
					(long) getpid(), (long) pid, WEXITSTATUS(waiter)))
				break;
		}
	} while (pid > 0);
}