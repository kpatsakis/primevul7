ServerItem::ServerItem(const QString &name, ItemType itype, const QString &continent, const QString &country) {
	siParent = NULL;
	bParent = true;
	qsName = name;
	itType = itype;
	if (itType == PublicType) {
		qsCountryCode = country;
		qsContinentCode = continent;
	}
	setFlags(flags() & ~Qt::ItemIsDragEnabled);
	bCA = false;

	init();
}