	string readMessageLine(Details &details) {
		TRACE_POINT();
		while (true) {
			string result = details.io.readLine(1024 * 16, &details.timeout);
			string line = result;
			if (!line.empty() && line[line.size() - 1] == '\n') {
				line.erase(line.size() - 1, 1);
			}

			if (result.empty()) {
				// EOF
				return result;
			} else if (startsWith(result, "!> ")) {
				P_DEBUG("[App " << details.pid << " stdout] " << line);
				result.erase(0, sizeof("!> ") - 1);
				return result;
			} else {
				if (details.stderrCapturer != NULL) {
					details.stderrCapturer->appendToBuffer(result);
				}
				LoggingKit::logAppOutput(details.pid, "stdout", line.data(), line.size());
			}
		}
	}