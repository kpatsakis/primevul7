void ConnectDialogEdit::showNotice(const QString &text) {
	QLabel *label = qwInlineNotice->findChild<QLabel *>(QLatin1String("qlPasteNotice"));
	Q_ASSERT(label);
	label->setText(text);
	qwInlineNotice->show();
	adjustSize();
}