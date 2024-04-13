ServerItem::ServerItem(const QString &name, const QString &host, unsigned short port, const QString &username, const QString &password) : QTreeWidgetItem(QTreeWidgetItem::UserType) {
	siParent = NULL;
	bParent = false;
	itType = FavoriteType;
	qsName = name;
	usPort = port;
	qsUsername = username;
	qsPassword = password;

	bCA = false;

	if (host.startsWith(QLatin1Char('@'))) {
		qsBonjourHost = host.mid(1);
		brRecord = BonjourRecord(qsBonjourHost, QLatin1String("_mumble._tcp."), QLatin1String("local."));
	} else {
		qsHostname = host;
	}

	init();
}