void ConnectDialog::stopDns(ServerItem *si) {
	if (!bAllowHostLookup) {
		return;
	}

	foreach(const ServerAddress &addr, si->qlAddresses) {
		if (qhPings.contains(addr)) {
			qhPings[addr].remove(si);
			if (qhPings[addr].isEmpty()) {
				qhPings.remove(addr);
				qhPingRand.remove(addr);
			}
		}
	}

	QString hostname = si->qsHostname.toLower();
	unsigned short port = si->usPort;
	UnresolvedServerAddress unresolved(hostname, port);

	if (qhDNSWait.contains(unresolved)) {
		qhDNSWait[unresolved].remove(si);
		if (qhDNSWait[unresolved].isEmpty()) {
			qhDNSWait.remove(unresolved);
			qlDNSLookup.removeAll(unresolved);
		}
	}
}