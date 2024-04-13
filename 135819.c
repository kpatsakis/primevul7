int do_diff_cache(const unsigned char *tree_sha1, struct diff_options *opt)
{
	struct tree *tree;
	struct rev_info revs;
	int i;
	struct cache_entry **dst;
	struct cache_entry *last = NULL;
	struct unpack_trees_options opts;
	struct tree_desc t;
	struct oneway_unpack_data unpack_cb;

	/*
	 * This is used by git-blame to run diff-cache internally;
	 * it potentially needs to repeatedly run this, so we will
	 * start by removing the higher order entries the last round
	 * left behind.
	 */
	dst = active_cache;
	for (i = 0; i < active_nr; i++) {
		struct cache_entry *ce = active_cache[i];
		if (ce_stage(ce)) {
			if (last && !strcmp(ce->name, last->name))
				continue;
			cache_tree_invalidate_path(active_cache_tree,
						   ce->name);
			last = ce;
			ce->ce_flags |= CE_REMOVE;
		}
		*dst++ = ce;
	}
	active_nr = dst - active_cache;

	init_revisions(&revs, NULL);
	revs.prune_data = opt->paths;
	tree = parse_tree_indirect(tree_sha1);
	if (!tree)
		die("bad tree object %s", sha1_to_hex(tree_sha1));

	unpack_cb.revs = &revs;
	unpack_cb.symcache[0] = '\0';
	memset(&opts, 0, sizeof(opts));
	opts.head_idx = 1;
	opts.index_only = 1;
	opts.merge = 1;
	opts.fn = oneway_diff;
	opts.unpack_data = &unpack_cb;
	opts.src_index = &the_index;
	opts.dst_index = &the_index;

	init_tree_desc(&t, tree->buffer, tree->size);
	if (unpack_trees(1, &t, &opts))
		exit(128);
	return 0;
}