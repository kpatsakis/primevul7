void ServerItem::hideCheck() {
	bool hide = false;
	bool ishidden = (parent() == NULL);

	if (! bParent && (itType == PublicType)) {
		if (g.s.ssFilter == Settings::ShowReachable)
			hide = (dPing == 0.0);
		else if (g.s.ssFilter == Settings::ShowPopulated)
			hide = (uiUsers == 0);
	}
	if (hide != ishidden) {
		if (hide)
			siParent->removeChild(this);
		else
			siParent->addChild(this);
	}
}