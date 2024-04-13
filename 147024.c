static void sigchld_handler(int x) {
	got_sigchld = 1;
}