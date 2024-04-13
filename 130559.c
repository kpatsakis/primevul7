ConnectDialog::~ConnectDialog() {
	ServerItem::qmIcons.clear();

	QList<FavoriteServer> ql;
	qmPingCache.clear();

	foreach(ServerItem *si, qlItems) {
		if (si->uiPing)
			qmPingCache.insert(UnresolvedServerAddress(si->qsHostname, si->usPort), si->uiPing);

		if (si->itType != ServerItem::FavoriteType)
			continue;
		ql << si->toFavoriteServer();
	}
	g.db->setFavorites(ql);
	g.db->setPingCache(qmPingCache);

	g.s.qbaConnectDialogHeader = qtwServers->header()->saveState();
	g.s.qbaConnectDialogGeometry = saveGeometry();
}