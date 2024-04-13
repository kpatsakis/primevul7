	void inferApplicationInfo(SpawnPreparationInfo &info) const {
		info.codeRevision = readFromRevisionFile(info);
		if (info.codeRevision.empty()) {
			info.codeRevision = inferCodeRevisionFromCapistranoSymlink(info);
		}
	}