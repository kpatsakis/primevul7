	void sendSpawnRequest(NegotiationDetails &details) {
		TRACE_POINT();
		try {
			const size_t UNIX_PATH_MAX = sizeof(((struct sockaddr_un *) 0)->sun_path);
			string data = "You have control 1.0\n"
				"passenger_root: " + config->resourceLocator->getInstallSpec() + "\n"
				"passenger_version: " PASSENGER_VERSION "\n"
				"ruby_libdir: " + config->resourceLocator->getRubyLibDir() + "\n"
				"gupid: " + details.gupid + "\n"
				"UNIX_PATH_MAX: " + toString(UNIX_PATH_MAX) + "\n";
			if (!details.options->apiKey.empty()) {
				data.append("connect_password: " + details.options->apiKey + "\n");
			}
			if (!config->instanceDir.empty()) {
				data.append("instance_dir: " + config->instanceDir + "\n");
				data.append("socket_dir: " + config->instanceDir + "/apps.s\n");
			}

			vector<string> args;
			vector<string>::const_iterator it, end;
			details.options->toVector(args, *config->resourceLocator, Options::SPAWN_OPTIONS);
			for (it = args.begin(); it != args.end(); it++) {
				const string &key = *it;
				it++;
				const string &value = *it;
				data.append(key + ": " + value + "\n");
			}

			vector<StaticString> lines;
			split(data, '\n', lines);
			foreach (const StaticString line, lines) {
				P_DEBUG("[App " << details.pid << " stdin >>] " << line);
			}
			writeExact(details.adminSocket, data, &details.timeout);
			writeExact(details.adminSocket, "\n", &details.timeout);
		} catch (const SystemException &e) {
			if (e.code() == EPIPE) {
				/* Ignore this. Process might have written an
				 * error response before reading the arguments,
				 * in which case we'll want to show that instead.
				 */
			} else {
				throw;
			}
		}
	}