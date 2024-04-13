	void handleSpawnErrorResponse(NegotiationDetails &details) {
		TRACE_POINT();
		map<string, string> attributes;

		while (true) {
			string line = readMessageLine(details);
			if (line.empty()) {
				throwAppSpawnException("An error occurred while starting the "
					"web application. It unexpected closed the connection while "
					"sending its startup response.",
					SpawnException::APP_STARTUP_PROTOCOL_ERROR,
					details);
			} else if (line[line.size() - 1] != '\n') {
				throwAppSpawnException("An error occurred while starting the "
					"web application. It sent a line without a newline character "
					"in its startup response.",
					SpawnException::APP_STARTUP_PROTOCOL_ERROR,
					details);
			} else if (line == "\n") {
				break;
			}

			string::size_type pos = line.find(": ");
			if (pos == string::npos) {
				throwAppSpawnException("An error occurred while starting the "
					"web application. It sent a startup response line without "
					"separator.",
					SpawnException::APP_STARTUP_PROTOCOL_ERROR,
					details);
			}

			string key = line.substr(0, pos);
			string value = line.substr(pos + 2, line.size() - pos - 3);
			attributes[key] = value;
		}

		try {
			string message = details.io.readAll(&details.timeout);
			SpawnException e("An error occured while starting the web application.",
				message,
				attributes["html"] == "true",
				SpawnException::APP_STARTUP_EXPLAINABLE_ERROR);
			annotateAppSpawnException(e, details);
			throwSpawnException(e, *details.options);
		} catch (const SystemException &e) {
			throwAppSpawnException("An error occurred while starting the "
				"web application. It tried to report an error message, but "
				"an I/O error occurred while reading this error message: " +
				e.sys(),
				SpawnException::APP_STARTUP_PROTOCOL_ERROR,
				details);
		} catch (const TimeoutException &) {
			throwAppSpawnException("An error occurred while starting the "
				"web application. It tried to report an error message, but "
				"it took too much time doing that.",
				SpawnException::APP_STARTUP_TIMEOUT,
				details);
		}
	}