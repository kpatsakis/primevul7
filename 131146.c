const char *get_git_work_tree(void)
{
	static int initialized = 0;
	if (!initialized) {
		work_tree = getenv(GIT_WORK_TREE_ENVIRONMENT);
		/* core.bare = true overrides implicit and config work tree */
		if (!work_tree && is_bare_repository_cfg < 1) {
			work_tree = git_work_tree_cfg;
			/* make_absolute_path also normalizes the path */
			if (work_tree && !is_absolute_path(work_tree))
				work_tree = xstrdup(make_absolute_path(git_path(work_tree)));
		} else if (work_tree)
			work_tree = xstrdup(make_absolute_path(work_tree));
		initialized = 1;
		if (work_tree)
			is_bare_repository_cfg = 0;
	}
	return work_tree;
}