	void setUlimits(const Options &options) {
		if (options.fileDescriptorUlimit != 0) {
			struct rlimit limit;
			int ret;

			limit.rlim_cur = options.fileDescriptorUlimit;
			limit.rlim_max = options.fileDescriptorUlimit;
			do {
				ret = setrlimit(RLIMIT_NOFILE, &limit);
			} while (ret == -1 && errno == EINTR);

			if (ret == -1) {
				int e = errno;
				fprintf(stderr, "Unable to set file descriptor ulimit to %u: %s (errno=%d)",
					options.fileDescriptorUlimit, strerror(e), e);
				fflush(stderr);
			}
		}
	}