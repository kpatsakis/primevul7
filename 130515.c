void ConnectDialogEdit::validate() {
	qsName = qleName->text().simplified();
	qsHostname = qleServer->text().simplified();
	usPort = qlePort->text().toUShort();
	qsUsername = qleUsername->text().simplified();
	qsPassword = qlePassword->text();

	// For bonjour hosts disable the port field as it's auto-detected
	qlePort->setDisabled(!qsHostname.isEmpty() && qsHostname.startsWith(QLatin1Char('@')));

	// For SuperUser show password edit
	if (qsUsername.toLower() == QLatin1String("superuser")) {
		qliPassword->setVisible(true);
		qlePassword->setVisible(true);
		qcbShowPassword->setVisible(true);
		adjustSize();
	} else if (qsPassword.isEmpty()) {
		qliPassword->setVisible(false);
		qlePassword->setVisible(false);
		qcbShowPassword->setVisible(false);
		adjustSize();
	}

	bOk = ! qsHostname.isEmpty() && ! qsUsername.isEmpty() && usPort;
	qdbbButtonBox->button(QDialogButtonBox::Ok)->setEnabled(bOk);
}