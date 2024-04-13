void ConnectDialog::onResolved(BonjourRecord record, QString host, int port) {
	qlBonjourActive.removeAll(record);
	foreach(ServerItem *si, qlItems) {
		if (si->brRecord == record) {
			unsigned short usport = static_cast<unsigned short>(port);
			if ((host != si->qsHostname) || (usport != si->usPort)) {
				stopDns(si);
				si->usPort = static_cast<unsigned short>(port);
				si->qsHostname = host;
				startDns(si);
			}
		}
	}
}