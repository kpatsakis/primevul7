void ConnectDialog::onLanResolveError(BonjourRecord br, DNSServiceErrorType err) {
	qlBonjourActive.removeAll(br);
	qWarning()<<"Bonjour reported resolver error "<< err;
}