abortcmd(
	int sig
	)
{
	if (current_output == stdout)
	    (void) fflush(stdout);
	putc('\n', stderr);
	(void) fflush(stderr);
	if (jump) longjmp(interrupt_buf, 1);
}