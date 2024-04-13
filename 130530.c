void ConnectDialogEdit::on_qcbShowPassword_toggled(bool checked) {
	qlePassword->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
}