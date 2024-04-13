	void setWorkingDirectory(const SpawnPreparationInfo &info) {
		vector<string>::const_iterator it, end = info.appRootPathsInsideChroot.end();
		int ret;

		for (it = info.appRootPathsInsideChroot.begin(); it != end; it++) {
			struct stat buf;
			ret = stat(it->c_str(), &buf);
			if (ret == -1 && errno == EACCES) {
				char parent[PATH_MAX];
				const char *end = strrchr(it->c_str(), '/');
				memcpy(parent, it->c_str(), end - it->c_str());
				parent[end - it->c_str()] = '\0';

				printf("!> Error\n");
				printf("!> \n");
				printf("This web application process is being run as user '%s' and group '%s' "
					"and must be able to access its application root directory '%s'. "
					"However, the parent directory '%s' has wrong permissions, thereby "
					"preventing this process from accessing its application root directory. "
					"Please fix the permissions of the directory '%s' first.\n",
					info.userSwitching.username.c_str(),
					info.userSwitching.groupname.c_str(),
					info.appRootPaths.back().c_str(),
					parent,
					parent);
				fflush(stdout);
				_exit(1);
			} else if (ret == -1) {
				int e = errno;
				printf("!> Error\n");
				printf("!> \n");
				printf("Unable to stat() directory '%s': %s (errno=%d)\n",
					it->c_str(), strerror(e), e);
				fflush(stdout);
				_exit(1);
			}
		}

		ret = chdir(info.appRootPathsInsideChroot.back().c_str());
		if (ret == 0) {
			setenv("PWD", info.appRootPathsInsideChroot.back().c_str(), 1);
		} else if (ret == -1 && errno == EACCES) {
			printf("!> Error\n");
			printf("!> \n");
			printf("This web application process is being run as user '%s' and group '%s' "
				"and must be able to access its application root directory '%s'. "
				"However this directory is not accessible because it has wrong permissions. "
				"Please fix these permissions first.\n",
				info.userSwitching.username.c_str(),
				info.userSwitching.groupname.c_str(),
				info.appRootPaths.back().c_str());
			fflush(stdout);
			_exit(1);
		} else {
			int e = errno;
			printf("!> Error\n");
			printf("!> \n");
			printf("Unable to change working directory to '%s': %s (errno=%d)\n",
				info.appRootPathsInsideChroot.back().c_str(), strerror(e), e);
			fflush(stdout);
			_exit(1);
		}
	}