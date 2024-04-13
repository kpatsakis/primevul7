ServerItem *ServerItem::fromUrl(QUrl url, QWidget *p) {
	if (! url.isValid() || (url.scheme() != QLatin1String("mumble"))) {
		return NULL;
	}

#if QT_VERSION >= 0x050000
	QUrlQuery query(url);
#endif

	if (url.userName().isEmpty()) {
		if (g.s.qsUsername.isEmpty()) {
			bool ok;
			QString defUserName = QInputDialog::getText(p, ConnectDialog::tr("Adding host %1").arg(url.host()), ConnectDialog::tr("Enter username"), QLineEdit::Normal, g.s.qsUsername, &ok).trimmed();
			if (! ok)
				return NULL;
			if (defUserName.isEmpty())
				return NULL;
			g.s.qsUsername = defUserName;
		}
		url.setUserName(g.s.qsUsername);
	}

#if QT_VERSION >= 0x050000
	ServerItem *si = new ServerItem(query.queryItemValue(QLatin1String("title")), url.host(), static_cast<unsigned short>(url.port(DEFAULT_MUMBLE_PORT)), url.userName(), url.password());

	if (query.hasQueryItem(QLatin1String("url")))
		si->qsUrl = query.queryItemValue(QLatin1String("url"));
#else
	ServerItem *si = new ServerItem(url.queryItemValue(QLatin1String("title")), url.host(), static_cast<unsigned short>(url.port(DEFAULT_MUMBLE_PORT)), url.userName(), url.password());

	if (url.hasQueryItem(QLatin1String("url")))
		si->qsUrl = url.queryItemValue(QLatin1String("url"));
#endif

	return si;
}