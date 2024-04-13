static void add_grep(struct rev_info *revs, const char *ptn, enum grep_pat_token what)
{
	if (!revs->grep_filter) {
		struct grep_opt *opt = xcalloc(1, sizeof(*opt));
		opt->status_only = 1;
		opt->pattern_tail = &(opt->pattern_list);
		opt->regflags = REG_NEWLINE;
		revs->grep_filter = opt;
	}
	append_grep_pattern(revs->grep_filter, ptn,
			    "command line", 0, what);
}