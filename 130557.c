void ConnectDialog::on_qaFavoriteAddNew_triggered() {
	ConnectDialogEdit *cde = new ConnectDialogEdit(this);

	if (cde->exec() == QDialog::Accepted) {
		ServerItem *si = new ServerItem(cde->qsName, cde->qsHostname, cde->usPort, cde->qsUsername, cde->qsPassword);
		qlItems << si;
		qtwServers->siFavorite->addServerItem(si);
		qtwServers->setCurrentItem(si);
		startDns(si);
	}
	delete cde;
}