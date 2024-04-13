void ServerView::fixupName(ServerItem *si) {
	QString name = si->qsName;

	int tag = 1;

	QRegExp tmatch(QLatin1String("(.+)\\((\\d+)\\)"));
	tmatch.setMinimal(true);
	if (tmatch.exactMatch(name)) {
		name = tmatch.capturedTexts().at(1).trimmed();
		tag = tmatch.capturedTexts().at(2).toInt();
	}

	bool found;
	QString cmpname;
	do {
		found = false;
		if (tag > 1)
			cmpname = name + QString::fromLatin1(" (%1)").arg(tag);
		else
			cmpname = name;

		foreach(ServerItem *f, siFavorite->qlChildren)
			if (f->qsName == cmpname)
				found = true;

		++tag;
	} while (found);

	si->qsName = cmpname;
}