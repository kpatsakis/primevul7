void ServerItem::addServerItem(ServerItem *childitem) {
	Q_ASSERT(childitem->siParent == NULL);

	childitem->siParent = this;
	qlChildren.append(childitem);
	childitem->hideCheck();

	if (bParent && (itType != PublicType) && isHidden())
		setHidden(false);
}