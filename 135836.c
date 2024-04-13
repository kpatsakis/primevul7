static int tree_entry_interesting(struct tree_desc *desc, const char *base, int baselen, struct diff_options *opt)
{
	const char *path;
	const unsigned char *sha1;
	unsigned mode;
	int i;
	int pathlen;
	int never_interesting = -1;

	if (!opt->nr_paths)
		return 1;

	sha1 = tree_entry_extract(desc, &path, &mode);

	pathlen = tree_entry_len(path, sha1);

	for (i = 0; i < opt->nr_paths; i++) {
		const char *match = opt->paths[i];
		int matchlen = opt->pathlens[i];
		int m = -1; /* signals that we haven't called strncmp() */

		if (baselen >= matchlen) {
			/* If it doesn't match, move along... */
			if (strncmp(base, match, matchlen))
				continue;

			/*
			 * The base is a subdirectory of a path which
			 * was specified, so all of them are interesting.
			 */
			return 2;
		}

		/* Does the base match? */
		if (strncmp(base, match, baselen))
			continue;

		match += baselen;
		matchlen -= baselen;

		if (never_interesting) {
			/*
			 * We have not seen any match that sorts later
			 * than the current path.
			 */

			/*
			 * Does match sort strictly earlier than path
			 * with their common parts?
			 */
			m = strncmp(match, path,
				    (matchlen < pathlen) ? matchlen : pathlen);
			if (m < 0)
				continue;

			/*
			 * If we come here even once, that means there is at
			 * least one pathspec that would sort equal to or
			 * later than the path we are currently looking at.
			 * In other words, if we have never reached this point
			 * after iterating all pathspecs, it means all
			 * pathspecs are either outside of base, or inside the
			 * base but sorts strictly earlier than the current
			 * one.  In either case, they will never match the
			 * subsequent entries.  In such a case, we initialized
			 * the variable to -1 and that is what will be
			 * returned, allowing the caller to terminate early.
			 */
			never_interesting = 0;
		}

		if (pathlen > matchlen)
			continue;

		if (matchlen > pathlen) {
			if (match[pathlen] != '/')
				continue;
			if (!S_ISDIR(mode))
				continue;
		}

		if (m == -1)
			/*
			 * we cheated and did not do strncmp(), so we do
			 * that here.
			 */
			m = strncmp(match, path, pathlen);

		/*
		 * If common part matched earlier then it is a hit,
		 * because we rejected the case where path is not a
		 * leading directory and is shorter than match.
		 */
		if (!m)
			return 1;
	}
	return never_interesting; /* No matches */
}