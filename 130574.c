void ConnectDialog::onUpdateLanList(const QList<BonjourRecord> &list) {
	QSet<ServerItem *> items;
	QSet<ServerItem *> old = qtwServers->siLAN->qlChildren.toSet();

	foreach(const BonjourRecord &record, list) {
		bool found = false;
		foreach(ServerItem *si, old) {
			if (si->brRecord == record) {
				items.insert(si);
				found = true;
				break;
			}
		}
		if (! found) {
			ServerItem *si = new ServerItem(record);
			qlItems << si;
			g.bc->bsrResolver->resolveBonjourRecord(record);
			startDns(si);
			qtwServers->siLAN->addServerItem(si);
		}
	}
	QSet<ServerItem *> remove = old.subtract(items);
	foreach(ServerItem *si, remove) {
		stopDns(si);
		qlItems.removeAll(si);
		delete si;
	}
}