static int parse_lldiff_command(const char *var, const char *ep, const char *value)
{
	const char *name;
	int namelen;
	struct ll_diff_driver *drv;

	name = var + 5;
	namelen = ep - name;
	for (drv = user_diff; drv; drv = drv->next)
		if (!strncmp(drv->name, name, namelen) && !drv->name[namelen])
			break;
	if (!drv) {
		drv = xcalloc(1, sizeof(struct ll_diff_driver));
		drv->name = xmemdupz(name, namelen);
		if (!user_diff_tail)
			user_diff_tail = &user_diff;
		*user_diff_tail = drv;
		user_diff_tail = &(drv->next);
	}

	return git_config_string(&(drv->cmd), var, value);
}