static int compare_tree_entry(struct tree_desc *t1, struct tree_desc *t2, const char *base, int baselen, struct diff_options *opt)
{
	unsigned mode1, mode2;
	const char *path1, *path2;
	const unsigned char *sha1, *sha2;
	int cmp, pathlen1, pathlen2;
	char *fullname;

	sha1 = tree_entry_extract(t1, &path1, &mode1);
	sha2 = tree_entry_extract(t2, &path2, &mode2);

	pathlen1 = tree_entry_len(path1, sha1);
	pathlen2 = tree_entry_len(path2, sha2);
	cmp = base_name_compare(path1, pathlen1, mode1, path2, pathlen2, mode2);
	if (cmp < 0) {
		show_entry(opt, "-", t1, base, baselen);
		return -1;
	}
	if (cmp > 0) {
		show_entry(opt, "+", t2, base, baselen);
		return 1;
	}
	if (!DIFF_OPT_TST(opt, FIND_COPIES_HARDER) && !hashcmp(sha1, sha2) && mode1 == mode2)
		return 0;

	/*
	 * If the filemode has changed to/from a directory from/to a regular
	 * file, we need to consider it a remove and an add.
	 */
	if (S_ISDIR(mode1) != S_ISDIR(mode2)) {
		show_entry(opt, "-", t1, base, baselen);
		show_entry(opt, "+", t2, base, baselen);
		return 0;
	}

	if (DIFF_OPT_TST(opt, RECURSIVE) && S_ISDIR(mode1)) {
		int retval;
		char *newbase = malloc_base(base, baselen, path1, pathlen1);
		if (DIFF_OPT_TST(opt, TREE_IN_RECURSIVE)) {
			newbase[baselen + pathlen1] = 0;
			opt->change(opt, mode1, mode2,
				    sha1, sha2, newbase);
			newbase[baselen + pathlen1] = '/';
		}
		retval = diff_tree_sha1(sha1, sha2, newbase, opt);
		free(newbase);
		return retval;
	}

	fullname = malloc_fullname(base, baselen, path1, pathlen1);
	opt->change(opt, mode1, mode2, sha1, sha2, fullname);
	free(fullname);
	return 0;
}