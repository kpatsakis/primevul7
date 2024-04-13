	Result handleSpawnResponse(NegotiationDetails &details) {
		TRACE_POINT();
		Json::Value sockets;
		Result result;

		while (true) {
			string line;

			try {
				line = readMessageLine(details);
			} catch (const SystemException &e) {
				throwAppSpawnException("An error occurred while starting the "
					"web application. There was an I/O error while reading its "
					"startup response: " + e.sys(),
					SpawnException::APP_STARTUP_PROTOCOL_ERROR,
					details);
			} catch (const TimeoutException &) {
				throwAppSpawnException("An error occurred while starting the "
					"web application: it did not write a startup response in time. "
					"If your app needs more time to start you can increase the "
					"Passenger start timeout config option.",
					SpawnException::APP_STARTUP_TIMEOUT,
					details);
			}

			if (line.empty()) {
				throwAppSpawnException("An error occurred while starting the "
					"web application. It unexpectedly closed the connection while "
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
			if (key == "socket") {
				// socket: <name>;<address>;<protocol>;<concurrency>
				// TODO: in case of TCP sockets, check whether it points to localhost
				// TODO: in case of unix sockets, check whether filename is absolute
				// and whether owner is correct
				vector<string> args;
				split(value, ';', args);
				if (args.size() == 4) {
					string error = validateSocketAddress(details, args[1]);
					if (!error.empty()) {
						throwAppSpawnException(
							"An error occurred while starting the web application. " + error,
							SpawnException::APP_STARTUP_PROTOCOL_ERROR,
							details);
					}

					Json::Value socket;
					socket["name"] = args[0];
					socket["address"] = fixupSocketAddress(*details.options, args[1]);
					socket["protocol"] = args[2];
					socket["concurrency"] = atoi(args[3]);
					sockets.append(socket);
				} else {
					throwAppSpawnException("An error occurred while starting the "
						"web application. It reported a wrongly formatted 'socket'"
						"response value: '" + value + "'",
						SpawnException::APP_STARTUP_PROTOCOL_ERROR,
						details);
				}
			} else if (key == "pid") {
				// pid: <PID>
				pid_t pid = atoi(value);
				ProcessMetricsCollector collector;
				vector<pid_t> pids;

				pids.push_back(pid);
				ProcessMetricMap metrics = collector.collect(pids);
				if (metrics[pid].uid != details.preparation->userSwitching.uid) {
					throwAppSpawnException("An error occurred while starting the "
						"web application. The PID that the loader has returned does "
						"not have the same UID as the loader itself.",
						SpawnException::APP_STARTUP_PROTOCOL_ERROR,
						details);
				}
				details.pid = pid;
			} else {
				throwAppSpawnException("An error occurred while starting the "
					"web application. It sent an unknown startup response line "
					"called '" + key + "'.",
					SpawnException::APP_STARTUP_PROTOCOL_ERROR,
					details);
			}
		}

		if (!hasSessionSockets(sockets)) {
			throwAppSpawnException("An error occured while starting the web "
				"application. It did not advertise any session sockets.",
				SpawnException::APP_STARTUP_PROTOCOL_ERROR,
				details);
		}

		result["type"] = "os_process";
		result["pid"] = (int) details.pid;
		result["gupid"] = details.gupid;
		result["sockets"] = sockets;
		result["code_revision"] = details.preparation->codeRevision;
		result["spawner_creation_time"] = (Json::UInt64) creationTime;
		result["spawn_start_time"] = (Json::UInt64) details.spawnStartTime;
		result.adminSocket = details.adminSocket;
		result.errorPipe = details.errorPipe;
		return result;
	}