void ConnectDialog::udpReply() {
	QUdpSocket *sock = qobject_cast<QUdpSocket *>(sender());

	while (sock->hasPendingDatagrams()) {
		char blob[64];

		QHostAddress host;
		unsigned short port;

		qint64 len = sock->readDatagram(blob+4, 24, &host, &port);
		if (len == 24) {
			if (host.scopeId() == QLatin1String("0"))
				host.setScopeId(QLatin1String(""));

			ServerAddress address(HostAddress(host), port);

			if (qhPings.contains(address)) {
				quint32 *ping = reinterpret_cast<quint32 *>(blob+4);
				quint64 *ts = reinterpret_cast<quint64 *>(blob+8);

				quint64 elapsed = tPing.elapsed() - (*ts ^ qhPingRand.value(address));

				foreach(ServerItem *si, qhPings.value(address)) {
					si->uiVersion = qFromBigEndian(ping[0]);
					quint32 users = qFromBigEndian(ping[3]);
					quint32 maxusers = qFromBigEndian(ping[4]);
					si->uiBandwidth = qFromBigEndian(ping[5]);

					if (! si->uiPingSort)
						si->uiPingSort = qmPingCache.value(UnresolvedServerAddress(si->qsHostname, si->usPort));

					si->setDatas(static_cast<double>(elapsed), users, maxusers);
					si->hideCheck();
				}
			}
		}
	}
}