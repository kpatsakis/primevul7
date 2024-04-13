	virtual void annotateAppSpawnException(SpawnException &e, NegotiationDetails &details) {
		if (details.debugDir != NULL) {
			e.addAnnotations(details.debugDir->readAll());
		}
	}