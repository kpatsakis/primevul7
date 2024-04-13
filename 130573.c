void ConnectDialog::on_qaFavoriteCopy_triggered() {
	ServerItem *si = static_cast<ServerItem *>(qtwServers->currentItem());
	if (! si)
		return;

	QApplication::clipboard()->setMimeData(si->toMimeData());
}