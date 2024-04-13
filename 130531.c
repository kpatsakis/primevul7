ServerItem::ServerItem(const ServerItem *si) {
	siParent = NULL;
	bParent = false;
	itType = FavoriteType;

	qsName = si->qsName;
	qsHostname = si->qsHostname;
	usPort = si->usPort;
	qsUsername = si->qsUsername;
	qsPassword = si->qsPassword;
	qsCountry = si->qsCountry;
	qsCountryCode = si->qsCountryCode;
	qsContinentCode = si->qsContinentCode;
	qsUrl = si->qsUrl;
	qsBonjourHost = si->qsBonjourHost;
	brRecord = si->brRecord;
	qlAddresses = si->qlAddresses;
	bCA = si->bCA;

	uiVersion = si->uiVersion;
	uiPing = si->uiPing;
	uiPingSort = si->uiPing;
	uiUsers = si->uiUsers;
	uiMaxUsers = si->uiMaxUsers;
	uiBandwidth = si->uiBandwidth;
	uiSent = si->uiSent;
	dPing = si->dPing;
	*asQuantile = * si->asQuantile;
}