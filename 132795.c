int ipmi_get_version(struct ipmi_user *user,
		     unsigned char *major,
		     unsigned char *minor)
{
	struct ipmi_device_id id;
	int rv, index;

	user = acquire_ipmi_user(user, &index);
	if (!user)
		return -ENODEV;

	rv = bmc_get_device_id(user->intf, NULL, &id, NULL, NULL);
	if (!rv) {
		*major = ipmi_version_major(&id);
		*minor = ipmi_version_minor(&id);
	}
	release_ipmi_user(user, index);

	return rv;
}