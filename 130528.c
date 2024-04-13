void ConnectDialog::OnSortChanged(int logicalIndex, Qt::SortOrder) {
	if (logicalIndex != 2) {
		return;
	}

	foreach(ServerItem *si, qlItems) {
		if (si->uiPing && (si->uiPing != si->uiPingSort)) {
			si->uiPingSort = si->uiPing;
			si->setDatas();
		}
	}
}