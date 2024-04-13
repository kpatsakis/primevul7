		DebugDir(uid_t uid, gid_t gid) {
			char buf[PATH_MAX] = "/tmp/passenger.spawn-debug.XXXXXXXXXX";
			const char *result = mkdtemp(buf);
			if (result == NULL) {
				int e = errno;
				throw SystemException("Cannot create a temporary directory "
					"in the format of '/tmp/passenger-spawn-debug.XXX'", e);
			} else {
				path = result;
				boost::this_thread::disable_interruption di;
				boost::this_thread::disable_syscall_interruption dsi;
				syscalls::chown(result, uid, gid);
			}
		}