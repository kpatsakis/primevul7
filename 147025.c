static void run_reboot_jobs(cron_db * db) {
	user *u;
	entry *e;
	int reboot;
	pid_t pid = getpid();

	/* lock exist - skip reboot jobs */
	if (access(REBOOT_LOCK, F_OK) == 0) {
		log_it("CRON", pid, "INFO",
			"@reboot jobs will be run at computer's startup.", 0);
		return;
	}
	/* lock doesn't exist - create lock, run reboot jobs */
	if ((reboot = creat(REBOOT_LOCK, S_IRUSR & S_IWUSR)) < 0)
		log_it("CRON", pid, "INFO", "Can't create lock for reboot jobs.",
			errno);
	else
		close(reboot);

	for (u = db->head; u != NULL; u = u->next) {
		for (e = u->crontab; e != NULL; e = e->next) {
			if (e->flags & WHEN_REBOOT)
				job_add(e, u);
		}
	}
	(void) job_runqueue();
}