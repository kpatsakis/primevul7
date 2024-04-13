int fpm_log_init_child(struct fpm_worker_pool_s *wp)  /* {{{ */
{
	if (!wp || !wp->config) {
		return -1;
	}

	if (wp->config->access_log && *wp->config->access_log) {
		if (wp->config->access_format) {
			fpm_log_format = strdup(wp->config->access_format);
		}
	}

	if (fpm_log_fd == -1) {
		fpm_log_fd = wp->log_fd;
	}


	for (wp = fpm_worker_all_pools; wp; wp = wp->next) {
		if (wp->log_fd > -1 && wp->log_fd != fpm_log_fd) {
			close(wp->log_fd);
			wp->log_fd = -1;
		}
	}

	return 0;
}