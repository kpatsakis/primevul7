void ConnectDialogEdit::accept() {
	validate();
	if (bOk) {
		QString server = qleServer->text().simplified();

		// If the user accidentally added a schema or path part, drop it now.
		// We can't do so during editing as that is quite jarring.
		const int schemaPos = server.indexOf(QLatin1String("://"));
		if (schemaPos != -1) {
			server.remove(0, schemaPos + 3);
		}

		const int pathPos = server.indexOf(QLatin1Char('/'));
		if (pathPos != -1) {
			server.resize(pathPos);
		}

		qleServer->setText(server);

		if (qleName->text().simplified().isEmpty() || !bCustomLabel) {
			qleName->setText(server);
		}

		QDialog::accept();
	}
}