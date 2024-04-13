	void throwAppSpawnException(const string &msg,
		SpawnException::ErrorKind errorKind,
		NegotiationDetails &details)
	{
		TRACE_POINT();
		// Stop the stderr capturing thread and get the captured stderr
		// output so far.
		string stderrOutput;
		if (details.stderrCapturer != NULL) {
			stderrOutput = details.stderrCapturer->stop();
		}

		// If the exception wasn't due to a timeout, try to capture the
		// remaining stderr output for at most 2 seconds.
		if (errorKind != SpawnException::PRELOADER_STARTUP_TIMEOUT
		 && errorKind != SpawnException::APP_STARTUP_TIMEOUT
		 && details.stderrCapturer != NULL)
		{
			bool done = false;
			unsigned long long timeout = 2000;
			while (!done) {
				char buf[1024 * 32];
				unsigned int ret;

				try {
					ret = readExact(details.stderrCapturer->getFd(), buf,
						sizeof(buf), &timeout);
					if (ret == 0) {
						done = true;
					} else {
						stderrOutput.append(buf, ret);
					}
				} catch (const SystemException &e) {
					P_WARN("Stderr I/O capture error: " << e.what());
					done = true;
				} catch (const TimeoutException &) {
					done = true;
				}
			}
		}
		details.stderrCapturer.reset();

		// Now throw SpawnException with the captured stderr output
		// as error response.
		SpawnException e(msg,
			createErrorPageFromStderrOutput(msg, errorKind, stderrOutput),
			true,
			errorKind);
		annotateAppSpawnException(e, details);
		throwSpawnException(e, *details.options);
	}