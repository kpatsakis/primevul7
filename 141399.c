	void prepareSwitchingWorkingDirectory(SpawnPreparationInfo &info, const Options &options) const {
		vector<string> components;
		split(info.appRootInsideChroot, '/', components);
		assert(components.front() == "");
		components.erase(components.begin());

		for (unsigned int i = 0; i < components.size(); i++) {
			string path;
			for (unsigned int j = 0; j <= i; j++) {
				path.append("/");
				path.append(components[j]);
			}
			if (path.empty()) {
				path = "/";
			}
			if (info.chrootDir == "/") {
				info.appRootPaths.push_back(path);
			} else {
				info.appRootPaths.push_back(info.chrootDir + path);
			}
			info.appRootPathsInsideChroot.push_back(path);
		}

		assert(info.appRootPathsInsideChroot.back() == info.appRootInsideChroot);
	}