void ConnectDialogEdit::init() {
	m_si = NULL;
	usPort = 0;
	bOk = true;
	bCustomLabel = false;

	qwInlineNotice->hide();

	qlePort->setValidator(new QIntValidator(1, 65535, qlePort));
	qlePort->setText(QString::number(DEFAULT_MUMBLE_PORT));
	qlePassword->setEchoMode(QLineEdit::Password);

	connect(qleName, SIGNAL(textChanged(const QString &)), this, SLOT(validate()));
	connect(qleServer, SIGNAL(textChanged(const QString &)), this, SLOT(validate()));
	connect(qlePort, SIGNAL(textChanged(const QString &)), this, SLOT(validate()));
	connect(qleUsername, SIGNAL(textChanged(const QString &)), this, SLOT(validate()));
	connect(qlePassword, SIGNAL(textChanged(const QString &)), this, SLOT(validate()));

	validate();
}