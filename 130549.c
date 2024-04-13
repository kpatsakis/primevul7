ServerItem::ServerItem(const BonjourRecord &br) : QTreeWidgetItem(QTreeWidgetItem::UserType) {
	siParent = NULL;
	bParent = false;
	itType = LANType;
	qsName = br.serviceName;
	qsBonjourHost = qsName;
	brRecord = br;
	usPort = 0;
	bCA = false;

	init();
}