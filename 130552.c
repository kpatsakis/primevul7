void ConnectDialog::on_qtwServers_customContextMenuRequested(const QPoint &mpos) {
	ServerItem *si = static_cast<ServerItem *>(qtwServers->itemAt(mpos));
	qmPopup->clear();

	if (si != NULL && si->bParent) {
		si = NULL;
	}
	
	if (si != NULL) {

		if (!g.s.disableConnectDialogEditing) {
			if (si->itType == ServerItem::FavoriteType) {
				qmPopup->addAction(qaFavoriteEdit);
				qmPopup->addAction(qaFavoriteRemove);
			} else {
				qmPopup->addAction(qaFavoriteAdd);
			}
		}
		
		if (!si->qsUrl.isEmpty()) {
			qmPopup->addAction(qaUrl);
		}
	}
	
	if (! qmPopup->isEmpty()) {
		qmPopup->addSeparator();
	}
	
	qmPopup->addMenu(qmFilters);

	qmPopup->popup(qtwServers->viewport()->mapToGlobal(mpos), NULL);
}