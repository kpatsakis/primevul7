QVariant ServerItem::data(int column, int role) const {
	if (bParent) {
		if (column == 0) {
			switch (role) {
				case Qt::DisplayRole:
					return qsName;
				case Qt::DecorationRole:
					if (itType == FavoriteType)
						return loadIcon(QLatin1String("skin:emblems/emblem-favorite.svg"));
					else if (itType == LANType)
						return loadIcon(QLatin1String("skin:places/network-workgroup.svg"));
					else if (! qsCountryCode.isEmpty()) {
						QString flag = QString::fromLatin1(":/flags/%1.svg").arg(qsCountryCode);
						if (!QFileInfo(flag).exists()) {
							flag = QLatin1String("skin:categories/applications-internet.svg");
						}
						return loadIcon(flag);
					}
					else
						return loadIcon(QLatin1String("skin:categories/applications-internet.svg"));
			}
		}
	} else {
		if (role == Qt::DisplayRole) {
			switch (column) {
				case 0:
					return qsName;
				case 1:
					return (dPing > 0.0) ? QString::number(uiPing) : QVariant();
				case 2:
					return uiUsers ? QString::fromLatin1("%1/%2 ").arg(uiUsers).arg(uiMaxUsers) : QVariant();
			}
		} else if (role == Qt::ToolTipRole) {
			QStringList qsl;
			foreach(const ServerAddress &addr, qlAddresses) {
				qsl << Qt::escape(addr.host.toString() + QLatin1String(":") + QString::number(static_cast<unsigned long>(addr.port)));
			}

			double ploss = 100.0;

			if (uiSent > 0)
				ploss = (uiSent - qMin(uiRecv, uiSent)) * 100. / uiSent;

			QString qs;
			qs +=
			    QLatin1String("<table>") +
			    QString::fromLatin1("<tr><th align=left>%1</th><td>%2</td></tr>").arg(ConnectDialog::tr("Servername"), Qt::escape(qsName)) +
			    QString::fromLatin1("<tr><th align=left>%1</th><td>%2</td></tr>").arg(ConnectDialog::tr("Hostname"), Qt::escape(qsHostname));

			if (! qsBonjourHost.isEmpty())
				qs += QString::fromLatin1("<tr><th align=left>%1</th><td>%2</td></tr>").arg(ConnectDialog::tr("Bonjour name"), Qt::escape(qsBonjourHost));

			qs +=
			    QString::fromLatin1("<tr><th align=left>%1</th><td>%2</td></tr>").arg(ConnectDialog::tr("Port")).arg(usPort) +
			    QString::fromLatin1("<tr><th align=left>%1</th><td>%2</td></tr>").arg(ConnectDialog::tr("Addresses"), qsl.join(QLatin1String(", ")));

			if (! qsUrl.isEmpty())
				qs += QString::fromLatin1("<tr><th align=left>%1</th><td>%2</td></tr>").arg(ConnectDialog::tr("Website"), Qt::escape(qsUrl));

			if (uiSent > 0) {
				qs += QString::fromLatin1("<tr><th align=left>%1</th><td>%2</td></tr>").arg(ConnectDialog::tr("Packet loss"), QString::fromLatin1("%1% (%2/%3)").arg(ploss, 0, 'f', 1).arg(uiRecv).arg(uiSent));
				if (uiRecv > 0) {
					qs +=
					    QString::fromLatin1("<tr><th align=left>%1</th><td>%2</td></tr>").arg(ConnectDialog::tr("Ping (80%)"), ConnectDialog::tr("%1 ms").
					            arg(boost::accumulators::extended_p_square(* asQuantile)[1] / 1000., 0, 'f', 2)) +
					    QString::fromLatin1("<tr><th align=left>%1</th><td>%2</td></tr>").arg(ConnectDialog::tr("Ping (95%)"), ConnectDialog::tr("%1 ms").
					            arg(boost::accumulators::extended_p_square(* asQuantile)[2] / 1000., 0, 'f', 2)) +
					    QString::fromLatin1("<tr><th align=left>%1</th><td>%2</td></tr>").arg(ConnectDialog::tr("Bandwidth"), ConnectDialog::tr("%1 kbit/s").arg(uiBandwidth / 1000)) +
					    QString::fromLatin1("<tr><th align=left>%1</th><td>%2</td></tr>").arg(ConnectDialog::tr("Users"), QString::fromLatin1("%1/%2").arg(uiUsers).arg(uiMaxUsers)) +
					    QString::fromLatin1("<tr><th align=left>%1</th><td>%2</td></tr>").arg(ConnectDialog::tr("Version")).arg(MumbleVersion::toString(uiVersion));
				}
			}
			qs += QLatin1String("</table>");
			return qs;
		} else if (role == Qt::BackgroundRole) {
			if (bCA) {
				QColor qc(Qt::green);
				qc.setAlpha(32);
				return qc;
			}
		}
	}
	return QTreeWidgetItem::data(column, role);
}