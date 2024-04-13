static void handle_signals(cron_db * database) {
	if (got_sighup) {
		got_sighup = 0;
#if defined WITH_INOTIFY
		/* watches must be reinstated on reload */
		if (inotify_enabled && (EnableClustering != 1)) {
			set_cron_unwatched(database->ifd);
			inotify_enabled = 0;
		}
#endif
		database->mtime = (time_t) 0;
		log_close();
	}

	if (got_sigchld) {
		got_sigchld = 0;
		sigchld_reaper();
	}
}