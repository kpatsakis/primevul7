	string createErrorPageFromStderrOutput(const string &msg,
		SpawnException::ErrorKind errorKind,
		const string &stderrOutput)
	{
		// These kinds of SpawnExceptions are not supposed to be handled through this function.
		assert(errorKind != SpawnException::PRELOADER_STARTUP_EXPLAINABLE_ERROR);
		assert(errorKind != SpawnException::APP_STARTUP_EXPLAINABLE_ERROR);

		string result = escapeHTML(msg);

		if (errorKind == SpawnException::PRELOADER_STARTUP_TIMEOUT
		 || errorKind == SpawnException::APP_STARTUP_TIMEOUT
		 || errorKind == SpawnException::PRELOADER_STARTUP_ERROR
		 || errorKind == SpawnException::APP_STARTUP_ERROR)
		{
			result.append(" Please read <a href=\"https://github.com/phusion/passenger/wiki/Debugging-application-startup-problems\">this article</a> "
				"for more information about this problem.");
		}
		result.append("<br>\n<h2>Raw process output:</h2>\n");

		if (strip(stderrOutput).empty()) {
			result.append("(empty)");
		} else {
			result.append("<pre>");
			result.append(escapeHTML(stderrOutput));
			result.append("</pre>");
		}

		return result;
	}