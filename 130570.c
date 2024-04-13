void ConnectDialogEdit::on_qbFill_clicked() {
	Q_ASSERT(m_si);

	qwInlineNotice->hide();
	adjustSize();

	qleName->setText(m_si->qsName);
	qleServer->setText(m_si->qsHostname);
	qleUsername->setText(m_si->qsUsername);
	qlePort->setText(QString::number(m_si->usPort));
	qlePassword->setText(m_si->qsPassword);

	delete m_si;
	m_si = NULL;
}