	static void nonInterruptableKillAndWaitpid(pid_t pid) {
		boost::this_thread::disable_syscall_interruption dsi;
		syscalls::kill(pid, SIGKILL);
		syscalls::waitpid(pid, NULL, 0);
	}