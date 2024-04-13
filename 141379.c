	void possiblyRaiseInternalError(const Options &options) {
		if (options.raiseInternalError) {
			throw RuntimeException("An internal error!");
		}
	}