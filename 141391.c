	string serializeEnvvarsFromPoolOptions(const Options &options) const {
		vector< pair<StaticString, StaticString> >::const_iterator it, end;
		string result;

		appendNullTerminatedKeyValue(result, "IN_PASSENGER", "1");
		appendNullTerminatedKeyValue(result, "PYTHONUNBUFFERED", "1");
		appendNullTerminatedKeyValue(result, "NODE_PATH", config->resourceLocator->getNodeLibDir());
		appendNullTerminatedKeyValue(result, "RAILS_ENV", options.environment);
		appendNullTerminatedKeyValue(result, "RACK_ENV", options.environment);
		appendNullTerminatedKeyValue(result, "WSGI_ENV", options.environment);
		appendNullTerminatedKeyValue(result, "NODE_ENV", options.environment);
		appendNullTerminatedKeyValue(result, "PASSENGER_APP_ENV", options.environment);
		if (!options.baseURI.empty() && options.baseURI != "/") {
			appendNullTerminatedKeyValue(result,
				"RAILS_RELATIVE_URL_ROOT",
				options.baseURI);
			appendNullTerminatedKeyValue(result,
				"RACK_BASE_URI",
				options.baseURI);
			appendNullTerminatedKeyValue(result,
				"PASSENGER_BASE_URI",
				options.baseURI);
		}

		string envvarsData;
		try {
			envvarsData = modp::b64_decode(options.environmentVariables.data(),
				options.environmentVariables.size());
		} catch (const std::runtime_error &) {
			P_WARN("Unable to decode base64-encoded environment variables: " <<
				options.environmentVariables);
			envvarsData.clear();
		}

		if (!envvarsData.empty()) {
			// The envvars data is in the same format as `result`,
			// so we can just append it.
			result.append(envvarsData);
		}

		try {
			return modp::b64_encode(result);
		} catch (const std::runtime_error &) {
			throw RuntimeException("Unable to encode data into a base64 string");
		}
	}