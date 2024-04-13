ServerItem *ServerItem::fromMimeData(const QMimeData *mime, bool default_name, QWidget *p, bool convertHttpUrls) {
	if (mime->hasFormat(QLatin1String("OriginatedInMumble")))
		return NULL;

	QUrl url;
	if (mime->hasUrls() && ! mime->urls().isEmpty())
		url = mime->urls().at(0);
	else if (mime->hasText())
		url = QUrl::fromEncoded(mime->text().toUtf8());

	QString qsFile = url.toLocalFile();
	if (! qsFile.isEmpty()) {
		QFile f(qsFile);
		// Make sure we don't accidently read something big the user
		// happened to have in his clipboard. We only want to look
		// at small link files.
		if (f.open(QIODevice::ReadOnly) && f.size() < 10240) {
			QByteArray qba = f.readAll();
			f.close();

			url = QUrl::fromEncoded(qba, QUrl::StrictMode);
			if (! url.isValid()) {
				// Windows internet shortcut files (.url) are an ini with an URL value
				QSettings qs(qsFile, QSettings::IniFormat);
				url = QUrl::fromEncoded(qs.value(QLatin1String("InternetShortcut/URL")).toByteArray(), QUrl::StrictMode);
			}
		}
	}

	if (default_name) {
#if QT_VERSION >= 0x050000
		QUrlQuery query(url);
		if (! query.hasQueryItem(QLatin1String("title"))) {
			query.addQueryItem(QLatin1String("title"), url.host());
		}
#else
		if (! url.hasQueryItem(QLatin1String("title"))) {
			url.addQueryItem(QLatin1String("title"), url.host());
		}
#endif
	}

	if (! url.isValid()) {
		return NULL;
	}

	// An URL from text without a scheme will have the hostname text
	// in the QUrl scheme and no hostname. We do not want to use that.
	if (url.host().isEmpty()) {
		return NULL;
	}

	// Some communication programs automatically create http links from domains.
	// When a user sends another user a domain to connect to, and http is added wrongly,
	// we do our best to remove it again.
	if (convertHttpUrls && (
	    url.scheme() == QLatin1String("http")
	    || url.scheme() == QLatin1String("https"))) {
		url.setScheme(QLatin1String("mumble"));
	}

	return fromUrl(url, p);
}