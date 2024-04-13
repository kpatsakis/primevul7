void ConnectDialog::on_qtwServers_currentItemChanged(QTreeWidgetItem *item, QTreeWidgetItem *) {
	ServerItem *si = static_cast<ServerItem *>(item);

	if (si->siParent == qtwServers->siFavorite) {
		qpbEdit->setEnabled(true);
	} else {
		qpbEdit->setEnabled(false);
	}
	
	bool bOk = !si->qlAddresses.isEmpty();
	if (!bAllowHostLookup) {
		bOk = true;
	}
	qdbbButtonBox->button(QDialogButtonBox::Ok)->setEnabled(bOk);

	bLastFound = true;
}