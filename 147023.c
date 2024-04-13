void set_cron_unwatched(int fd) {
	int i;

	for (i = 0; i < sizeof (wd) / sizeof (wd[0]); ++i) {
		if (wd[i] < 0) {
			inotify_rm_watch(fd, wd[i]);
			wd[i] = -1;
		}
	}
}