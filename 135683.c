static void show_new_file(struct oneway_unpack_data *cbdata,
			  struct cache_entry *new,
			  int cached, int match_missing)
{
	const unsigned char *sha1;
	unsigned int mode;
	struct rev_info *revs = cbdata->revs;

	/*
	 * New file in the index: it might actually be different in
	 * the working copy.
	 */
	if (get_stat_data(new, &sha1, &mode, cached, match_missing, cbdata) < 0)
		return;

	diff_index_show_file(revs, "+", new, sha1, mode);
}