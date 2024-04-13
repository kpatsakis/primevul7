	void switchUser(const SpawnPreparationInfo &info) {
		if (info.userSwitching.enabled) {
			enterLveJail(&info.userSwitching.lveUserPwd);

			bool setgroupsCalled = false;
			#ifdef HAVE_GETGROUPLIST
				if (info.userSwitching.ngroups <= NGROUPS_MAX) {
					setgroupsCalled = true;
					if (setgroups(info.userSwitching.ngroups,
						info.userSwitching.gidset.get()) == -1)
					{
						int e = errno;
						printf("!> Error\n");
						printf("!> \n");
						printf("setgroups(%d, ...) failed: %s (errno=%d)\n",
							info.userSwitching.ngroups, strerror(e), e);
						fflush(stdout);
						_exit(1);
					}
				}
			#endif
			if (!setgroupsCalled && initgroups(info.userSwitching.username.c_str(),
				info.userSwitching.gid) == -1)
			{
				int e = errno;
				printf("!> Error\n");
				printf("!> \n");
				printf("initgroups() failed: %s (errno=%d)\n",
					strerror(e), e);
				fflush(stdout);
				_exit(1);
			}
			if (setgid(info.userSwitching.gid) == -1) {
				int e = errno;
				printf("!> Error\n");
				printf("!> \n");
				printf("setgid() failed: %s (errno=%d)\n",
					strerror(e), e);
				fflush(stdout);
				_exit(1);
			}
			if (setuid(info.userSwitching.uid) == -1) {
				int e = errno;
				printf("!> Error\n");
				printf("!> \n");
				printf("setuid() failed: %s (errno=%d)\n",
					strerror(e), e);
				fflush(stdout);
				_exit(1);
			}

			// We set these environment variables here instead of
			// in the SpawnPreparer because SpawnPreparer might
			// be executed by bash, but these environment variables
			// must be set before bash.
			setenv("USER", info.userSwitching.username.c_str(), 1);
			setenv("LOGNAME", info.userSwitching.username.c_str(), 1);
			setenv("SHELL", info.userSwitching.shell.c_str(), 1);
			setenv("HOME", info.userSwitching.home.c_str(), 1);
			// The application root may contain one or more symlinks
			// in its path. If the application calls getcwd(), it will
			// get the resolved path.
			//
			// It turns out that there is no such thing as a path without
			// unresolved symlinks. The shell presents a working directory with
			// unresolved symlinks (which it calls the "logical working directory"),
			// but that is an illusion provided by the shell. The shell reports
			// the logical working directory though the PWD environment variable.
			//
			// See also:
			// https://github.com/phusion/passenger/issues/1596#issuecomment-138154045
			// http://git.savannah.gnu.org/cgit/coreutils.git/tree/src/pwd.c
			// http://www.opensource.apple.com/source/shell_cmds/shell_cmds-170/pwd/pwd.c
			setenv("PWD", info.appRoot.c_str(), 1);
		}
	}