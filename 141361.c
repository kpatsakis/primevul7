	static void checkChrootDirectories(const Options &options) {
		if (!options.preexecChroot.empty()) {
			// TODO: check whether appRoot is a child directory of preexecChroot
			// and whether postexecChroot is a child directory of appRoot.
		}
	}