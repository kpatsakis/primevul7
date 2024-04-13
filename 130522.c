ServerItem *ServerView::getParent(const QString &continentcode, const QString &countrycode, const QString &countryname, const QString &usercontinent, const QString &usercountry) {
	ServerItem *continent = qmContinent.value(continentcode);
	if (!continent) {
		QString name = qmContinentNames.value(continentcode);
		if (name.isEmpty())
			name = continentcode;
		continent = new ServerItem(name, ServerItem::PublicType, continentcode);
		qmContinent.insert(continentcode, continent);
		siPublic->addServerItem(continent);

		if (!continentcode.isEmpty()) {
			if (continentcode == usercontinent) {
				continent->setExpanded(true);
				scrollToItem(continent, QAbstractItemView::PositionAtTop);
			}
		} else {
			continent->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
		}
	}

	// If the continent code is empty, we put the server directly into the "Unknown" continent
	if (continentcode.isEmpty()) {
		return continent;
	}

	ServerItem *country = qmCountry.value(countrycode);
	if (!country) {
		country = new ServerItem(countryname, ServerItem::PublicType, continentcode, countrycode);
		qmCountry.insert(countrycode, country);
		country->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);

		continent->addServerItem(country);

		if (!countrycode.isEmpty() && countrycode == usercountry) {
			country->setExpanded(true);
			scrollToItem(country, QAbstractItemView::PositionAtTop);
		}
	}
	return country;
}