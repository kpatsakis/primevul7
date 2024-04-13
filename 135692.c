void diff_tree_release_paths(struct diff_options *opt)
{
	free(opt->pathlens);
}