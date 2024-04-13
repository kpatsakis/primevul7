	void setChroot(const SpawnPreparationInfo &info) {
		if (info.chrootDir != "/") {
			int ret = chroot(info.chrootDir.c_str());
			if (ret == -1) {
				int e = errno;
				fprintf(stderr, "Cannot chroot() to '%s': %s (errno=%d)\n",
					info.chrootDir.c_str(),
					strerror(e),
					e);
				fflush(stderr);
				_exit(1);
			}

			ret = chdir("/");
			if (ret == -1) {
				int e = errno;
				fprintf(stderr, "Cannot chdir(\"/\") inside chroot: %s (errno=%d)\n",
					strerror(e),
					e);
				fflush(stderr);
				_exit(1);
			}
		}
	}