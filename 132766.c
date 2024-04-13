int ipmi_get_maintenance_mode(struct ipmi_user *user)
{
	int mode, index;
	unsigned long flags;

	user = acquire_ipmi_user(user, &index);
	if (!user)
		return -ENODEV;

	spin_lock_irqsave(&user->intf->maintenance_mode_lock, flags);
	mode = user->intf->maintenance_mode;
	spin_unlock_irqrestore(&user->intf->maintenance_mode_lock, flags);
	release_ipmi_user(user, index);

	return mode;
}