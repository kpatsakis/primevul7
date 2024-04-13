QMimeData *ServerView::mimeData(const QList<QTreeWidgetItem *> mimeitems) const {
	if (mimeitems.isEmpty())
		return NULL;

	ServerItem *si = static_cast<ServerItem *>(mimeitems.first());
	return si->toMimeData();
}