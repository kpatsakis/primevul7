void ConnectDialog::onFiltersTriggered(QAction *act) {
	if (act == qaShowAll)
		g.s.ssFilter = Settings::ShowAll;
	else if (act == qaShowReachable)
		g.s.ssFilter = Settings::ShowReachable;
	else if (act == qaShowPopulated)
		g.s.ssFilter = Settings::ShowPopulated;

	foreach(ServerItem *si, qlItems)
		si->hideCheck();
}