void ConnectDialog::timeTick() {
	if (! bLastFound && ! g.s.qsLastServer.isEmpty()) {
		QList<QTreeWidgetItem *> items = qtwServers->findItems(g.s.qsLastServer, Qt::MatchExactly | Qt::MatchRecursive);
		if (!items.isEmpty()) {
			bLastFound = true;
			qtwServers->setCurrentItem(items.at(0));
			if (g.s.bAutoConnect && bAutoConnect) {
				siAutoConnect = static_cast<ServerItem *>(items.at(0));
				if (! siAutoConnect->qlAddresses.isEmpty()) {
					accept();
					return;
				} else if (!bAllowHostLookup) {
					accept();
					return;
				}
			}
		}
	}

	if (bAllowHostLookup) {
		// Start DNS Lookup of first unknown hostname
		foreach(const UnresolvedServerAddress &unresolved, qlDNSLookup) {
			if (qsDNSActive.contains(unresolved)) {
				continue;
			}

			qlDNSLookup.removeAll(unresolved);
			qlDNSLookup.append(unresolved);

			qsDNSActive.insert(unresolved);
			ServerResolver *sr = new ServerResolver();
			QObject::connect(sr, SIGNAL(resolved()), this, SLOT(lookedUp()));
			sr->resolve(unresolved.hostname, unresolved.port);
			break;
		}
	}

	ServerItem *current = static_cast<ServerItem *>(qtwServers->currentItem());
	ServerItem *hover = static_cast<ServerItem *>(qtwServers->itemAt(qtwServers->viewport()->mapFromGlobal(QCursor::pos())));

	ServerItem *si = NULL;

	if (tCurrent.elapsed() >= 1000000ULL)
		si = current;
	if (! si && (tHover.elapsed() >= 1000000ULL))
		si = hover;

	if (si) {
		QString hostname = si->qsHostname.toLower();
		unsigned short port = si->usPort;
		UnresolvedServerAddress unresolved(hostname, port);

		if (si->qlAddresses.isEmpty()) {
			if (! hostname.isEmpty()) {
				qlDNSLookup.removeAll(unresolved);
				qlDNSLookup.prepend(unresolved);
			}
			si = NULL;
		}
	}

	if (!si) {
		if (qlItems.isEmpty())
			return;

		bool expanded;

		do {
			++iPingIndex;
			if (iPingIndex >= qlItems.count()) {
				if (tRestart.isElapsed(1000000ULL))
					iPingIndex = 0;
				else
					return;
			}
			si = qlItems.at(iPingIndex);

			ServerItem *p = si->siParent;
			expanded = true;
			while (p && expanded) {
				expanded = expanded && p->isExpanded();
				p = p->siParent;
			}
		} while (si->qlAddresses.isEmpty() || ! expanded);
	}

	if (si == current)
		tCurrent.restart();
	if (si == hover)
		tHover.restart();

	foreach(const ServerAddress &addr, si->qlAddresses) {
		sendPing(addr.host.toAddress(), addr.port);
	}
}