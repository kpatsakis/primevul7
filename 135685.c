static void remove_tempfile_on_signal(int signo)
{
	remove_tempfile();
	signal(SIGINT, SIG_DFL);
	raise(signo);
}