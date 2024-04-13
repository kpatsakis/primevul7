	string inferCodeRevisionFromCapistranoSymlink(const SpawnPreparationInfo &info) const {
		if (extractBaseName(info.appRoot) == "current") {
			char buf[PATH_MAX + 1];
			ssize_t ret;

			do {
				ret = readlink(info.appRoot.c_str(), buf, PATH_MAX);
			} while (ret == -1 && errno == EINTR);
			if (ret == -1) {
				if (errno == EINVAL) {
					return string();
				} else {
					int e = errno;
					P_WARN("Cannot read symlink " << info.appRoot << ": " << strerror(e));
				}
			}

			buf[ret] = '\0';
			return extractBaseName(buf);
		} else {
			return string();
		}
	}