static const char *external_diff_attr(const char *name)
{
	struct git_attr_check attr_diff_check;

	if (!name)
		return NULL;

	setup_diff_attr_check(&attr_diff_check);
	if (!git_checkattr(name, 1, &attr_diff_check)) {
		const char *value = attr_diff_check.value;
		if (!ATTR_TRUE(value) &&
		    !ATTR_FALSE(value) &&
		    !ATTR_UNSET(value)) {
			struct ll_diff_driver *drv;

			for (drv = user_diff; drv; drv = drv->next)
				if (!strcmp(drv->name, value))
					return drv->cmd;
		}
	}
	return NULL;
}