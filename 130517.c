void ConnectDialog::fillList() {
	QList<QTreeWidgetItem *> ql;
	QList<QTreeWidgetItem *> qlNew;

	foreach(const PublicInfo &pi, qlPublicServers) {
		bool found = false;
		foreach(ServerItem *si, qlItems) {
			if ((pi.qsIp == si->qsHostname) && (pi.usPort == si->usPort)) {
				si->qsCountry = pi.qsCountry;
				si->qsCountryCode = pi.qsCountryCode;
				si->qsContinentCode = pi.qsContinentCode;
				si->qsUrl = pi.quUrl.toString();
				si->bCA = pi.bCA;
				si->setDatas();

				if (si->itType == ServerItem::PublicType)
					found = true;
			}
		}
		if (! found)
			ql << new ServerItem(pi);
	}

	while (! ql.isEmpty()) {
		ServerItem *si = static_cast<ServerItem *>(ql.takeAt(qrand() % ql.count()));
		qlNew << si;
		qlItems << si;
	}

	foreach(QTreeWidgetItem *qtwi, qlNew) {
		ServerItem *si = static_cast<ServerItem *>(qtwi);
		ServerItem *p = qtwServers->getParent(si->qsContinentCode, si->qsCountryCode, si->qsCountry, qsUserContinentCode, qsUserCountryCode);
		p->addServerItem(si);

		if (p->isExpanded() && p->parent()->isExpanded())
			startDns(si);
	}
}