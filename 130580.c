void ConnectDialog::on_qaFavoriteEdit_triggered() {
	ServerItem *si = static_cast<ServerItem *>(qtwServers->currentItem());
	if (! si || (si->itType != ServerItem::FavoriteType))
		return;

	QString host;
	if (! si->qsBonjourHost.isEmpty())
		host = QLatin1Char('@') + si->qsBonjourHost;
	else
		host = si->qsHostname;

	ConnectDialogEdit *cde = new ConnectDialogEdit(this, si->qsName, host, si->qsUsername, si->usPort, si->qsPassword);

	if (cde->exec() == QDialog::Accepted) {

		si->qsName = cde->qsName;
		si->qsUsername = cde->qsUsername;
		si->qsPassword = cde->qsPassword;
		if ((cde->qsHostname != host) || (cde->usPort != si->usPort)) {
			stopDns(si);

			si->qlAddresses.clear();
			si->reset();

			si->usPort = cde->usPort;

			if (cde->qsHostname.startsWith(QLatin1Char('@'))) {
				si->qsHostname = QString();
				si->qsBonjourHost = cde->qsHostname.mid(1);
				si->brRecord = BonjourRecord(si->qsBonjourHost, QLatin1String("_mumble._tcp."), QLatin1String("local."));
			} else {
				si->qsHostname = cde->qsHostname;
				si->qsBonjourHost = QString();
				si->brRecord = BonjourRecord();
			}
			startDns(si);
		}
		si->setDatas();
	}
	delete cde;
}