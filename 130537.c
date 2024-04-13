void ConnectDialogEdit::on_qleServer_textEdited(const QString& server) {
	// If using automatic label, update it
	if (!bCustomLabel) {
		qleName->setText(server);
	}
}