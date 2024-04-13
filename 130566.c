bool ConnectDialogEdit::updateFromClipboard() {
	delete m_si;
	m_si = ServerItem::fromMimeData(QApplication::clipboard()->mimeData(), false, NULL, true);
	bool hasServerData = m_si != NULL;
	if (hasServerData) {
		showNotice(tr("You have an URL in your clipboard.\nDo you want to fill the dialog with this data?\nHost: %1 Port: %2").arg(m_si->qsHostname).arg(m_si->usPort));
		return true;
	} else {
		qwInlineNotice->hide();
		adjustSize();
		return false;
	}
}