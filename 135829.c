int diff_tree(struct tree_desc *t1, struct tree_desc *t2, const char *base, struct diff_options *opt)
{
	int baselen = strlen(base);

	for (;;) {
		if (DIFF_OPT_TST(opt, QUIET) && DIFF_OPT_TST(opt, HAS_CHANGES))
			break;
		if (opt->nr_paths) {
			skip_uninteresting(t1, base, baselen, opt);
			skip_uninteresting(t2, base, baselen, opt);
		}
		if (!t1->size) {
			if (!t2->size)
				break;
			show_entry(opt, "+", t2, base, baselen);
			update_tree_entry(t2);
			continue;
		}
		if (!t2->size) {
			show_entry(opt, "-", t1, base, baselen);
			update_tree_entry(t1);
			continue;
		}
		switch (compare_tree_entry(t1, t2, base, baselen, opt)) {
		case -1:
			update_tree_entry(t1);
			continue;
		case 0:
			update_tree_entry(t1);
			/* Fallthrough */
		case 1:
			update_tree_entry(t2);
			continue;
		}
		die("git-diff-tree: internal error");
	}
	return 0;
}