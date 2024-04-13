	bool hasSessionSockets(const Json::Value &sockets) const {
		Json::Value::const_iterator it, end = sockets.end();

		for (it = sockets.begin(); it != end; it++) {
			const Json::Value &socket = *it;
			if (socket["protocol"] == "session" || socket["protocol"] == "http_session") {
				return true;
			}
		}
		return false;
	}