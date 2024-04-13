void ConnectDialog::on_qaFavoritePaste_triggered() {
	ServerItem *si = ServerItem::fromMimeData(QApplication::clipboard()->mimeData());
	if (! si)
		return;

	qlItems << si;
	qtwServers->siFavorite->addServerItem(si);
	qtwServers->setCurrentItem(si);
	startDns(si);
}