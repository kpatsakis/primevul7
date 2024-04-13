	void prepareChroot(SpawnPreparationInfo &info, const Options &options) {
		TRACE_POINT();
		info.appRoot = absolutizePath(options.appRoot);
		if (options.preexecChroot.empty()) {
			info.chrootDir = "/";
		} else {
			info.chrootDir = absolutizePath(options.preexecChroot);
		}
		if (info.appRoot != info.chrootDir && startsWith(info.appRoot, info.chrootDir + "/")) {
			SpawnException e("Invalid configuration: '" + info.chrootDir +
				"' has been configured as the chroot jail, but the application " +
				"root directory '" + info.appRoot + "' is not a subdirectory of the " +
				"chroot directory, which it must be.");
			throwSpawnException(e, options);
		}
		if (info.appRoot == info.chrootDir) {
			info.appRootInsideChroot = "/";
		} else if (info.chrootDir == "/") {
			info.appRootInsideChroot = info.appRoot;
		} else {
			info.appRootInsideChroot = info.appRoot.substr(info.chrootDir.size());
		}
	}