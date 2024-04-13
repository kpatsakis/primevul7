ConnectDialogEdit::ConnectDialogEdit(QWidget *parent) : QDialog(parent) {
	setupUi(this);
	setWindowTitle(tr("Add Server"));
	init();

	if (!updateFromClipboard()) {
		// If connected to a server assume the user wants to add it
		if (g.sh && g.sh->isRunning()) {
			QString host, name, user, pw;
			unsigned short port = DEFAULT_MUMBLE_PORT;

			g.sh->getConnectionInfo(host, port, user, pw);
			Channel *c = Channel::get(0);
			if (c && c->qsName != QLatin1String("Root")) {
				name = c->qsName;
			}

			showNotice(tr("You are currently connected to a server.\nDo you want to fill the dialog with the connection data of this server?\nHost: %1 Port: %2").arg(host).arg(port));
			m_si = new ServerItem(name, host, port, user, pw);
		}
	}
	qleUsername->setText(g.s.qsUsername);
}