	void handleInvalidSpawnResponseType(const string &line, NegotiationDetails &details) {
		if (line.empty()) {
			throwAppSpawnException("An error occurred while starting "
				"the web application. It exited before signalling successful "
				"startup back to " PROGRAM_NAME ".",
				SpawnException::APP_STARTUP_ERROR,
				details);
		} else {
			throwAppSpawnException("An error occurred while starting "
				"the web application. It sent an unknown response type \"" +
				cEscapeString(line) + "\".",
				SpawnException::APP_STARTUP_PROTOCOL_ERROR,
				details);
		}
	}