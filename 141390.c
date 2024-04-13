	string readFromRevisionFile(const SpawnPreparationInfo &info) const {
		string filename = info.appRoot + "/REVISION";
		try {
			if (fileExists(filename)) {
				return strip(readAll(filename));
			}
		} catch (const SystemException &e) {
			P_WARN("Cannot access " << filename << ": " << e.what());
		}
		return string();
	}