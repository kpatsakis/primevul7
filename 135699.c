static int commit_match(struct commit *commit, struct rev_info *opt)
{
	if (!opt->grep_filter)
		return 1;
	return grep_buffer(opt->grep_filter,
			   NULL, /* we say nothing, not even filename */
			   commit->buffer, strlen(commit->buffer));
}