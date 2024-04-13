static bool current_user_in_group(gid_t gid)
{
	int i;

	for (i = 0; i < current_user.ut.ngroups; i++) {
		if (current_user.ut.groups[i] == gid) {
			return True;
		}
	}

	return False;
}