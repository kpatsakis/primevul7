static char **get_groupnames(void)
{
	int ngroups;
	gid_t *group_ids;
	int ret, i;
	char **groupnames;
	struct group *gr;

	ngroups = getgroups(0, NULL);
	if (ngroups < 0) {
		usernic_error("Failed to get number of groups the user "
			      "belongs to: %s\n", strerror(errno));
		return NULL;
	}
	if (ngroups == 0)
		return NULL;

	group_ids = malloc(sizeof(gid_t) * ngroups);
	if (!group_ids) {
		usernic_error("Failed to allocate memory while getting groups "
			      "the user belongs to: %s\n",
			      strerror(errno));
		return NULL;
	}

	ret = getgroups(ngroups, group_ids);
	if (ret < 0) {
		free(group_ids);
		usernic_error("Failed to get process groups: %s\n",
			      strerror(errno));
		return NULL;
	}

	groupnames = malloc(sizeof(char *) * (ngroups + 1));
	if (!groupnames) {
		free(group_ids);
		usernic_error("Failed to allocate memory while getting group "
			      "names: %s\n",
			      strerror(errno));
		return NULL;
	}

	memset(groupnames, 0, sizeof(char *) * (ngroups + 1));

	for (i = 0; i < ngroups; i++) {
		gr = getgrgid(group_ids[i]);
		if (!gr) {
			usernic_error("Failed to get group name: %s\n",
				      strerror(errno));
			free(group_ids);
			free_groupnames(groupnames);
			return NULL;
		}

		groupnames[i] = strdup(gr->gr_name);
		if (!groupnames[i]) {
			usernic_error("Failed to copy group name \"%s\"",
				      gr->gr_name);
			free(group_ids);
			free_groupnames(groupnames);
			return NULL;
		}
	}

	free(group_ids);

	return groupnames;
}