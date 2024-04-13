void diff_tree_setup_paths(const char **p, struct diff_options *opt)
{
	opt->nr_paths = 0;
	opt->pathlens = NULL;
	opt->paths = NULL;

	if (p) {
		int i;

		opt->paths = p;
		opt->nr_paths = count_paths(p);
		if (opt->nr_paths == 0) {
			opt->pathlens = NULL;
			return;
		}
		opt->pathlens = xmalloc(opt->nr_paths * sizeof(int));
		for (i=0; i < opt->nr_paths; i++)
			opt->pathlens[i] = strlen(p[i]);
	}
}