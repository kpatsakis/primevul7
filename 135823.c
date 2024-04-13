int run_diff_index(struct rev_info *revs, int cached)
{
	struct object *ent;
	struct tree *tree;
	const char *tree_name;
	struct unpack_trees_options opts;
	struct tree_desc t;
	struct oneway_unpack_data unpack_cb;

	mark_merge_entries();

	ent = revs->pending.objects[0].item;
	tree_name = revs->pending.objects[0].name;
	tree = parse_tree_indirect(ent->sha1);
	if (!tree)
		return error("bad tree object %s", tree_name);

	unpack_cb.revs = revs;
	unpack_cb.symcache[0] = '\0';
	memset(&opts, 0, sizeof(opts));
	opts.head_idx = 1;
	opts.index_only = cached;
	opts.merge = 1;
	opts.fn = oneway_diff;
	opts.unpack_data = &unpack_cb;
	opts.src_index = &the_index;
	opts.dst_index = NULL;

	init_tree_desc(&t, tree->buffer, tree->size);
	if (unpack_trees(1, &t, &opts))
		exit(128);

	diffcore_std(&revs->diffopt);
	diff_flush(&revs->diffopt);
	return 0;
}