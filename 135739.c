static int get_stat_data(struct cache_entry *ce,
			 const unsigned char **sha1p,
			 unsigned int *modep,
			 int cached, int match_missing,
			 struct oneway_unpack_data *cbdata)
{
	const unsigned char *sha1 = ce->sha1;
	unsigned int mode = ce->ce_mode;

	if (!cached) {
		int changed;
		struct stat st;
		changed = check_removed(ce, &st);
		if (changed < 0)
			return -1;
		else if (changed) {
			if (match_missing) {
				*sha1p = sha1;
				*modep = mode;
				return 0;
			}
			return -1;
		}
		changed = ce_match_stat(ce, &st, 0);
		if (changed) {
			mode = ce_mode_from_stat(ce, st.st_mode);
			sha1 = null_sha1;
		}
	}

	*sha1p = sha1;
	*modep = mode;
	return 0;
}