void ConnectDialog::initList() {
	if (bPublicInit || (qlPublicServers.count() > 0))
		return;

	bPublicInit = true;

	QUrl url;
	url.setPath(QLatin1String("/v1/list"));
#if QT_VERSION >= 0x050000
	QUrlQuery query;
	query.addQueryItem(QLatin1String("version"), QLatin1String(MUMTEXT(MUMBLE_VERSION_STRING)));
	url.setQuery(query);
#else
	url.addQueryItem(QLatin1String("version"), QLatin1String(MUMTEXT(MUMBLE_VERSION_STRING)));
#endif

	WebFetch::fetch(QLatin1String("publist"), url, this, SLOT(fetched(QByteArray,QUrl,QMap<QString,QString>)));
}