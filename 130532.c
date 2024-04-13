void ConnectDialog::on_qtwServers_itemExpanded(QTreeWidgetItem *item) {
	if (qtwServers->siPublic != NULL && item == qtwServers->siPublic) {
		initList();
		fillList();
	}

	ServerItem *p = static_cast<ServerItem *>(item);

	foreach(ServerItem *si, p->qlChildren)
		startDns(si);
}