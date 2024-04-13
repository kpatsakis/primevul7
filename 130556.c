ConnectDialogEdit::ConnectDialogEdit(QWidget *p, const QString &name, const QString &host, const QString &user, unsigned short port, const QString &password) : QDialog(p) {
	setupUi(this);
	init();

	bCustomLabel = ! name.simplified().isEmpty();

	qleName->setText(name);
	qleServer->setText(host);
	qleUsername->setText(user);
	qlePort->setText(QString::number(port));
	qlePassword->setText(password);

	validate();
}