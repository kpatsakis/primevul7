void ConnectDialog::on_qaUrl_triggered() {
	auto *si = static_cast< const ServerItem * >(qtwServers->currentItem());
	if (!si || si->qsUrl.isEmpty()) {
		return;
	}

	const QStringList allowedSchemes = { QLatin1String("http"), QLatin1String("https") };

	const auto url = QUrl(si->qsUrl);
	if (allowedSchemes.contains(url.scheme())) {
		QDesktopServices::openUrl(url);
	} else {
		// Inform user that the requested URL has been blocked
		QMessageBox msgBox;
		msgBox.setText(QObject::tr("<b>Blocked URL scheme \"%1\"</b>").arg(url.scheme()));
		msgBox.setInformativeText(QObject::tr("The URL uses a scheme that has been blocked for security reasons."));
		msgBox.setDetailedText(QObject::tr("Blocked URL: \"%1\"").arg(url.toString()));
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
	}
}