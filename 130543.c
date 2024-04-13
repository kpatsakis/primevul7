void ConnectDialog::startDns(ServerItem *si) {
	if (!bAllowHostLookup) {
		return;
	}

	QString hostname = si->qsHostname.toLower();
	unsigned short port = si->usPort;
	UnresolvedServerAddress unresolved(hostname, port);

	if (si->qlAddresses.isEmpty()) {
		// Determine if qsHostname is an IP address
		// or a hostname. If it is an IP address, we
		// can treat it as resolved as-is.
		QHostAddress qha(si->qsHostname);
		bool hostnameIsIPAddress = !qha.isNull();
		if (hostnameIsIPAddress) {
			si->qlAddresses.append(ServerAddress(HostAddress(qha), port));
		} else {
			si->qlAddresses = qhDNSCache.value(unresolved);
		}
	}

	if (qtwServers->currentItem() == si)
		qdbbButtonBox->button(QDialogButtonBox::Ok)->setEnabled(! si->qlAddresses.isEmpty());

	if (! si->qlAddresses.isEmpty()) {
		foreach(const ServerAddress &addr, si->qlAddresses) {
			qhPings[addr].insert(si);
		}
		return;
	}

#ifdef USE_BONJOUR
	if (bAllowBonjour && si->qsHostname.isEmpty() && ! si->brRecord.serviceName.isEmpty()) {
		if (! qlBonjourActive.contains(si->brRecord)) {
			g.bc->bsrResolver->resolveBonjourRecord(si->brRecord);
			qlBonjourActive.append(si->brRecord);
		}
		return;
	}
#endif

	if (! qhDNSWait.contains(unresolved)) {
		if (si->itType == ServerItem::PublicType)
			qlDNSLookup.append(unresolved);
		else
			qlDNSLookup.prepend(unresolved);
	}
	qhDNSWait[unresolved].insert(si);
}