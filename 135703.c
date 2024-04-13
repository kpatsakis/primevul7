void diffcore_std(struct diff_options *options)
{
	if (DIFF_OPT_TST(options, QUIET))
		return;

	if (options->skip_stat_unmatch && !DIFF_OPT_TST(options, FIND_COPIES_HARDER))
		diffcore_skip_stat_unmatch(options);
	if (options->break_opt != -1)
		diffcore_break(options->break_opt);
	if (options->detect_rename)
		diffcore_rename(options);
	if (options->break_opt != -1)
		diffcore_merge_broken();
	if (options->pickaxe)
		diffcore_pickaxe(options->pickaxe, options->pickaxe_opts);
	if (options->orderfile)
		diffcore_order(options->orderfile);
	diff_resolve_rename_copy();
	diffcore_apply_filter(options->filter);

	if (diff_queued_diff.nr)
		DIFF_OPT_SET(options, HAS_CHANGES);
	else
		DIFF_OPT_CLR(options, HAS_CHANGES);
}