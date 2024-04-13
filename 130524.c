ServerView::ServerView(QWidget *p) : QTreeWidget(p) {
	siFavorite = new ServerItem(tr("Favorite"), ServerItem::FavoriteType);
	addTopLevelItem(siFavorite);
	siFavorite->setExpanded(true);
	siFavorite->setHidden(true);

#ifdef USE_BONJOUR
	siLAN = new ServerItem(tr("LAN"), ServerItem::LANType);
	addTopLevelItem(siLAN);
	siLAN->setExpanded(true);
	siLAN->setHidden(true);
#else
	siLAN = NULL;
#endif

	if (!g.s.disablePublicList) {
		siPublic = new ServerItem(tr("Public Internet"), ServerItem::PublicType);
		siPublic->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
		addTopLevelItem(siPublic);

		siPublic->setExpanded(false);

		// The continent code is empty when the server's IP address is not in the GeoIP database
		qmContinentNames.insert(QLatin1String(""), tr("Unknown"));

		qmContinentNames.insert(QLatin1String("af"), tr("Africa"));
		qmContinentNames.insert(QLatin1String("as"), tr("Asia"));
		qmContinentNames.insert(QLatin1String("na"), tr("North America"));
		qmContinentNames.insert(QLatin1String("sa"), tr("South America"));
		qmContinentNames.insert(QLatin1String("eu"), tr("Europe"));
		qmContinentNames.insert(QLatin1String("oc"), tr("Oceania"));
	} else {
		qWarning()<< "Public list disabled";

		siPublic = NULL;
	}
}