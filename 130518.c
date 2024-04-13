void ConnectDialog::fetched(QByteArray xmlData, QUrl, QMap<QString, QString> headers) {
	if (xmlData.isNull()) {
		QMessageBox::warning(this, QLatin1String("Mumble"), tr("Failed to fetch server list"), QMessageBox::Ok);
		return;
	}

	QDomDocument doc;
	doc.setContent(xmlData);

	qlPublicServers.clear();
	qsUserCountry = headers.value(QLatin1String("Geo-Country"));
	qsUserCountryCode = headers.value(QLatin1String("Geo-Country-Code")).toLower();
	qsUserContinentCode = headers.value(QLatin1String("Geo-Continent-Code")).toLower();

	QDomElement root=doc.documentElement();
	QDomNode n = root.firstChild();
	while (!n.isNull()) {
		QDomElement e = n.toElement();
		if (!e.isNull()) {
			if (e.tagName() == QLatin1String("server")) {
				PublicInfo pi;
				pi.qsName = e.attribute(QLatin1String("name"));
				pi.quUrl = e.attribute(QLatin1String("url"));
				pi.qsIp = e.attribute(QLatin1String("ip"));
				pi.usPort = e.attribute(QLatin1String("port")).toUShort();
				pi.qsCountry = e.attribute(QLatin1String("country"), tr("Unknown"));
				pi.qsCountryCode = e.attribute(QLatin1String("country_code")).toLower();
				pi.qsContinentCode = e.attribute(QLatin1String("continent_code")).toLower();
				pi.bCA = e.attribute(QLatin1String("ca")).toInt() ? true : false;

				qlPublicServers << pi;
			}
		}
		n = n.nextSibling();
	}

	tPublicServers.restart();

	fillList();
}