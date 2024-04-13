static void usage(void) {
	const char **dflags;

	fprintf(stderr, "usage:  %s [-h] print this message \n \
		[-i] deamon runs without inotify support \n \
		[-m <mail command>] off or specify prefered client for sending mails \n \
		[-n] run in foreground \n \
		[-p] permit any crontab \n \
		[-c] enable clustering support \n \
		[-s] log into syslog instead of sending mails \n \
		[-x [",
		ProgramName);
	for (dflags = DebugFlagNames; *dflags; dflags++)
		fprintf(stderr, "%s%s", *dflags, dflags[1] ? "," : "]");
	fprintf(stderr, "] print debug information\n");
	exit(ERROR_EXIT);
}