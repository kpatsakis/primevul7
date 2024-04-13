	string validateSocketAddress(NegotiationDetails &details, const string &_address) const {
		string address = _address;
		stringstream error;

		switch (getSocketAddressType(address)) {
		case SAT_UNIX: {
			address = fixupSocketAddress(*details.options, address);
			string filename = parseUnixSocketAddress(address);

			// Verify that the socket filename is absolute.
			if (!isAbsolutePath(filename)) {
				error << "It reported a non-absolute socket filename: \"" <<
					cEscapeString(filename) << "\"";
				break;
			}

			// Verify that the process owns the socket.
			struct stat buf;
			if (lstat(filename.c_str(), &buf) == -1) {
				int e = errno;
				error << "It reported an inaccessible socket filename: \"" <<
					cEscapeString(filename) << "\" (lstat() failed with errno " <<
					e << ": " << strerror(e) << ")";
				break;
			}
			if (buf.st_uid != details.preparation->userSwitching.uid) {
				error << "It advertised a Unix domain socket that has a different " <<
					"owner than expected (should be UID " << details.preparation->userSwitching.uid <<
					", but actual UID was " << buf.st_uid << ")";
				break;
			}
			break;
		}
		case SAT_TCP:
			// TODO: validate that the socket is localhost.
			break;
		default:
			error << "It reported an unsupported socket address type: \"" <<
				cEscapeString(address) << "\"";
			break;
		}

		return error.str();
	}