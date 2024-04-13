	void throwSpawnException(SpawnException &e, const Options &options) {
		if (config->errorHandler != NULL) {
			config->errorHandler(config, e, options);
		}
		throw e;
	}