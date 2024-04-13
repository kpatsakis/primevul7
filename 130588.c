void ConnectDialog::on_qaFavoriteAdd_triggered() {
	ServerItem *si = static_cast<ServerItem *>(qtwServers->currentItem());
	if (! si || (si->itType == ServerItem::FavoriteType))
		return;

	si = new ServerItem(si);
	qtwServers->fixupName(si);
	qlItems << si;
	qtwServers->siFavorite->addServerItem(si);
	qtwServers->setCurrentItem(si);
	startDns(si);
}