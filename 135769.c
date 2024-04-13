static void show_tree(struct diff_options *opt, const char *prefix, struct tree_desc *desc, const char *base, int baselen)
{
	int all_interesting = 0;
	while (desc->size) {
		int show;

		if (all_interesting)
			show = 1;
		else {
			show = tree_entry_interesting(desc, base, baselen,
						      opt);
			if (show == 2)
				all_interesting = 1;
		}
		if (show < 0)
			break;
		if (show)
			show_entry(opt, prefix, desc, base, baselen);
		update_tree_entry(desc);
	}
}