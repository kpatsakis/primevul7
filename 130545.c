void ConnectDialog::lookedUp() {
	ServerResolver *sr = qobject_cast<ServerResolver *>(QObject::sender());
	sr->deleteLater();

	QString hostname = sr->hostname().toLower();
	unsigned short port = sr->port();
	UnresolvedServerAddress unresolved(hostname, port);

	qsDNSActive.remove(unresolved);

	// An error occurred, or no records were found.
	if (sr->records().size() == 0) {
		return;
	}

	QSet<ServerAddress> qs;
	foreach (ServerResolverRecord record, sr->records()) {
		foreach(const HostAddress &ha, record.addresses()) {
			qs.insert(ServerAddress(ha, record.port()));
		}
	}

	QSet<ServerItem *> waiting = qhDNSWait[unresolved];
	foreach(ServerItem *si, waiting) {
		foreach (const ServerAddress &addr, qs) {
			qhPings[addr].insert(si);
		}

		si->qlAddresses = qs.toList();
	}

	qlDNSLookup.removeAll(unresolved);
	qhDNSCache.insert(unresolved, qs.toList());
	qhDNSWait.remove(unresolved);

	foreach(ServerItem *si, waiting) {
		if (si == qtwServers->currentItem()) {
			on_qtwServers_currentItemChanged(si, si);
			if (si == siAutoConnect)
				accept();
		}
	}

	if (bAllowPing) {
		foreach(const ServerAddress &addr, qs) {
			sendPing(addr.host.toAddress(), addr.port);
		}
	}
}