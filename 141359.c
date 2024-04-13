	static string fixupSocketAddress(const Options &options, const string &address) {
		TRACE_POINT();
		if (!options.preexecChroot.empty() && !options.postexecChroot.empty()) {
			ServerAddressType type = getSocketAddressType(address);
			if (type == SAT_UNIX) {
				string filename = parseUnixSocketAddress(address);
				string fixedAddress = "unix:";
				if (!options.preexecChroot.empty()) {
					fixedAddress.append(options.preexecChroot.data(),
						options.preexecChroot.size());
				}
				if (!options.postexecChroot.empty()) {
					fixedAddress.append(options.postexecChroot.data(),
						options.postexecChroot.size());
				}
				fixedAddress.append(filename);
				return fixedAddress;
			} else {
				return address;
			}
		} else {
			return address;
		}
	}