static void skip_uninteresting(struct tree_desc *t, const char *base, int baselen, struct diff_options *opt)
{
	int all_interesting = 0;
	while (t->size) {
		int show;

		if (all_interesting)
			show = 1;
		else {
			show = tree_entry_interesting(t, base, baselen, opt);
			if (show == 2)
				all_interesting = 1;
		}
		if (!show) {
			update_tree_entry(t);
			continue;
		}
		/* Skip it all? */
		if (show < 0)
			t->size = 0;
		return;
	}
}