bool ServerView::dropMimeData(QTreeWidgetItem *, int, const QMimeData *mime, Qt::DropAction) {
	ServerItem *si = ServerItem::fromMimeData(mime);
	if (! si)
		return false;

	fixupName(si);

	qobject_cast<ConnectDialog *>(parent())->qlItems << si;
	siFavorite->addServerItem(si);

	qobject_cast<ConnectDialog *>(parent())->startDns(si);

	setCurrentItem(si);

	return true;
}