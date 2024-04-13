void diff_setup(struct diff_options *options)
{
	memset(options, 0, sizeof(*options));

	options->file = stdout;

	options->line_termination = '\n';
	options->break_opt = -1;
	options->rename_limit = -1;
	options->dirstat_percent = 3;
	options->context = 3;

	options->change = diff_change;
	options->add_remove = diff_addremove;
	if (diff_use_color_default > 0)
		DIFF_OPT_SET(options, COLOR_DIFF);
	else
		DIFF_OPT_CLR(options, COLOR_DIFF);
	options->detect_rename = diff_detect_rename_default;

	options->a_prefix = "a/";
	options->b_prefix = "b/";
}