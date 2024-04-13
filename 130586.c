ConnectDialog::ConnectDialog(QWidget *p, bool autoconnect) : QDialog(p), bAutoConnect(autoconnect) {
	setupUi(this);
#ifdef Q_OS_MAC
	setWindowModality(Qt::WindowModal);
#endif
	bPublicInit = false;

	siAutoConnect = NULL;

	bAllowPing = g.s.ptProxyType == Settings::NoProxy;
	bAllowHostLookup = g.s.ptProxyType == Settings::NoProxy;
	bAllowBonjour = g.s.ptProxyType == Settings::NoProxy;
	bAllowFilters = g.s.ptProxyType == Settings::NoProxy;

	if (tPublicServers.elapsed() >= 60 * 24 * 1000000ULL) {
		qlPublicServers.clear();
	}

	qdbbButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	qdbbButtonBox->button(QDialogButtonBox::Ok)->setText(tr("&Connect"));

	QPushButton *qpbAdd = new QPushButton(tr("&Add New..."), this);
	qpbAdd->setDefault(false);
	qpbAdd->setAutoDefault(false);
	connect(qpbAdd, SIGNAL(clicked()), qaFavoriteAddNew, SIGNAL(triggered()));
	qdbbButtonBox->addButton(qpbAdd, QDialogButtonBox::ActionRole);

	
	qpbEdit = new QPushButton(tr("&Edit..."), this);
	qpbEdit->setEnabled(false);
	qpbEdit->setDefault(false);
	qpbEdit->setAutoDefault(false);
	connect(qpbEdit, SIGNAL(clicked()), qaFavoriteEdit, SIGNAL(triggered()));
	qdbbButtonBox->addButton(qpbEdit, QDialogButtonBox::ActionRole);
	
	qpbAdd->setHidden(g.s.disableConnectDialogEditing);
	qpbEdit->setHidden(g.s.disableConnectDialogEditing);

	qtwServers->setItemDelegate(new ServerViewDelegate());

	// Hide ping and user count if we are not allowed to ping.
	if (!bAllowPing) {
		qtwServers->setColumnCount(1);
	}

	qtwServers->sortItems(1, Qt::AscendingOrder);

#if QT_VERSION >= 0x050000
	qtwServers->header()->setSectionResizeMode(0, QHeaderView::Stretch);
	if (qtwServers->columnCount() >= 2) {
		qtwServers->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	}
	if (qtwServers->columnCount() >= 3) {
		qtwServers->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	}
#else
	qtwServers->header()->setResizeMode(0, QHeaderView::Stretch);
	if (qtwServers->columnCount() >= 2) {
		qtwServers->header()->setResizeMode(1, QHeaderView::ResizeToContents);
	}
	if (qtwServers->columnCount() >= 3) {
		qtwServers->header()->setResizeMode(2, QHeaderView::ResizeToContents);
	}
#endif

	connect(qtwServers->header(), SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)), this, SLOT(OnSortChanged(int, Qt::SortOrder)));

	qaShowAll->setChecked(false);
	qaShowReachable->setChecked(false);
	qaShowPopulated->setChecked(false);

	if (bAllowFilters) {
		switch (g.s.ssFilter) {
			case Settings::ShowPopulated:
				qaShowPopulated->setChecked(true);
				break;
			case Settings::ShowReachable:
				qaShowReachable->setChecked(true);
				break;
			default:
				qaShowAll->setChecked(true);
				break;
		}
	} else {
		qaShowAll->setChecked(true);
	}

	qagFilters = new QActionGroup(this);
	qagFilters->addAction(qaShowAll);
	qagFilters->addAction(qaShowReachable);
	qagFilters->addAction(qaShowPopulated);

	connect(qagFilters, SIGNAL(triggered(QAction *)), this, SLOT(onFiltersTriggered(QAction *)));

	qmPopup = new QMenu(this);
	qmFilters = new QMenu(tr("&Filters"), this);
	qmFilters->addAction(qaShowAll);
	qmFilters->addAction(qaShowReachable);
	qmFilters->addAction(qaShowPopulated);

	if (!bAllowFilters) {
		qmFilters->setEnabled(false);
	}

	QList<QTreeWidgetItem *> ql;
	QList<FavoriteServer> favorites = g.db->getFavorites();

	foreach(const FavoriteServer &fs, favorites) {
		ServerItem *si = new ServerItem(fs);
		qlItems << si;
		startDns(si);
		qtwServers->siFavorite->addServerItem(si);
	}

#ifdef USE_BONJOUR
	// Make sure the we got the objects we need, then wire them up
	if (bAllowBonjour && g.bc->bsbBrowser && g.bc->bsrResolver) {
		connect(g.bc->bsbBrowser, SIGNAL(error(DNSServiceErrorType)),
		        this, SLOT(onLanBrowseError(DNSServiceErrorType)));
		connect(g.bc->bsbBrowser, SIGNAL(currentBonjourRecordsChanged(const QList<BonjourRecord> &)),
		        this, SLOT(onUpdateLanList(const QList<BonjourRecord> &)));
		connect(g.bc->bsrResolver, SIGNAL(error(BonjourRecord, DNSServiceErrorType)),
		        this, SLOT(onLanResolveError(BonjourRecord, DNSServiceErrorType)));
		connect(g.bc->bsrResolver, SIGNAL(bonjourRecordResolved(BonjourRecord, QString, int)),
		        this, SLOT(onResolved(BonjourRecord, QString, int)));
		onUpdateLanList(g.bc->bsbBrowser->currentRecords());
	}
#endif

	qtPingTick = new QTimer(this);
	connect(qtPingTick, SIGNAL(timeout()), this, SLOT(timeTick()));

	qusSocket4 = new QUdpSocket(this);
	qusSocket6 = new QUdpSocket(this);
	bIPv4 = qusSocket4->bind(QHostAddress(QHostAddress::Any), 0);
	bIPv6 = qusSocket6->bind(QHostAddress(QHostAddress::AnyIPv6), 0);
	connect(qusSocket4, SIGNAL(readyRead()), this, SLOT(udpReply()));
	connect(qusSocket6, SIGNAL(readyRead()), this, SLOT(udpReply()));

	if (qtwServers->siFavorite->isHidden()
	    && (!qtwServers->siLAN || qtwServers->siLAN->isHidden())
	    && qtwServers->siPublic != NULL) {
		qtwServers->siPublic->setExpanded(true);
	}

	iPingIndex = -1;
	qtPingTick->start(50);

	new QShortcut(QKeySequence(QKeySequence::Copy), this, SLOT(on_qaFavoriteCopy_triggered()));
	new QShortcut(QKeySequence(QKeySequence::Paste), this, SLOT(on_qaFavoritePaste_triggered()));

	qtwServers->setCurrentItem(NULL);
	bLastFound = false;

	qmPingCache = g.db->getPingCache();

	if (! g.s.qbaConnectDialogGeometry.isEmpty())
		restoreGeometry(g.s.qbaConnectDialogGeometry);
	if (! g.s.qbaConnectDialogHeader.isEmpty())
		qtwServers->header()->restoreState(g.s.qbaConnectDialogHeader);
}