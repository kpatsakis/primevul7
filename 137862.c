int fpm_log_open(int reopen) /* {{{ */
{
	struct fpm_worker_pool_s *wp;
	int ret = 1;

	int fd;
	for (wp = fpm_worker_all_pools; wp; wp = wp->next) {
		if (!wp->config->access_log) {
			continue;
		}
		ret = 0;

		fd = open(wp->config->access_log, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
		if (0 > fd) {
			zlog(ZLOG_SYSERROR, "failed to open access log (%s)", wp->config->access_log);
			return -1;
		} else {
			zlog(ZLOG_DEBUG, "open access log (%s)", wp->config->access_log);
		}

		if (reopen) {
			dup2(fd, wp->log_fd);
			close(fd);
			fd = wp->log_fd;
			fpm_pctl_kill_all(SIGQUIT);
		} else {
			wp->log_fd = fd;
		}

		if (0 > fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC)) {
			zlog(ZLOG_WARNING, "failed to change attribute of access_log");
		}
	}

	return ret;
}