static int check_removed(const struct cache_entry *ce, struct stat *st)
{
	if (lstat(ce->name, st) < 0) {
		if (errno != ENOENT && errno != ENOTDIR)
			return -1;
		return 1;
	}
	if (has_symlink_leading_path(ce_namelen(ce), ce->name))
		return 1;
	if (S_ISDIR(st->st_mode)) {
		unsigned char sub[20];

		/*
		 * If ce is already a gitlink, we can have a plain
		 * directory (i.e. the submodule is not checked out),
		 * or a checked out submodule.  Either case this is not
		 * a case where something was removed from the work tree,
		 * so we will return 0.
		 *
		 * Otherwise, if the directory is not a submodule
		 * repository, that means ce which was a blob turned into
		 * a directory --- the blob was removed!
		 */
		if (!S_ISGITLINK(ce->ce_mode) &&
		    resolve_gitlink_ref(ce->name, "HEAD", sub))
			return 1;
	}
	return 0;
}