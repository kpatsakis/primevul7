static void quit(int x) {
	(void) unlink(_PATH_CRON_PID);
	_exit(0);
}