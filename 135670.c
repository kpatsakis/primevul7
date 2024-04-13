int diff_opt_parse(struct diff_options *options, const char **av, int ac)
{
	const char *arg = av[0];

	/* Output format options */
	if (!strcmp(arg, "-p") || !strcmp(arg, "-u"))
		options->output_format |= DIFF_FORMAT_PATCH;
	else if (opt_arg(arg, 'U', "unified", &options->context))
		options->output_format |= DIFF_FORMAT_PATCH;
	else if (!strcmp(arg, "--raw"))
		options->output_format |= DIFF_FORMAT_RAW;
	else if (!strcmp(arg, "--patch-with-raw"))
		options->output_format |= DIFF_FORMAT_PATCH | DIFF_FORMAT_RAW;
	else if (!strcmp(arg, "--numstat"))
		options->output_format |= DIFF_FORMAT_NUMSTAT;
	else if (!strcmp(arg, "--shortstat"))
		options->output_format |= DIFF_FORMAT_SHORTSTAT;
	else if (opt_arg(arg, 'X', "dirstat", &options->dirstat_percent))
		options->output_format |= DIFF_FORMAT_DIRSTAT;
	else if (!strcmp(arg, "--cumulative"))
		options->output_format |= DIFF_FORMAT_CUMULATIVE;
	else if (!strcmp(arg, "--check"))
		options->output_format |= DIFF_FORMAT_CHECKDIFF;
	else if (!strcmp(arg, "--summary"))
		options->output_format |= DIFF_FORMAT_SUMMARY;
	else if (!strcmp(arg, "--patch-with-stat"))
		options->output_format |= DIFF_FORMAT_PATCH | DIFF_FORMAT_DIFFSTAT;
	else if (!strcmp(arg, "--name-only"))
		options->output_format |= DIFF_FORMAT_NAME;
	else if (!strcmp(arg, "--name-status"))
		options->output_format |= DIFF_FORMAT_NAME_STATUS;
	else if (!strcmp(arg, "-s"))
		options->output_format |= DIFF_FORMAT_NO_OUTPUT;
	else if (!prefixcmp(arg, "--stat")) {
		char *end;
		int width = options->stat_width;
		int name_width = options->stat_name_width;
		arg += 6;
		end = (char *)arg;

		switch (*arg) {
		case '-':
			if (!prefixcmp(arg, "-width="))
				width = strtoul(arg + 7, &end, 10);
			else if (!prefixcmp(arg, "-name-width="))
				name_width = strtoul(arg + 12, &end, 10);
			break;
		case '=':
			width = strtoul(arg+1, &end, 10);
			if (*end == ',')
				name_width = strtoul(end+1, &end, 10);
		}

		/* Important! This checks all the error cases! */
		if (*end)
			return 0;
		options->output_format |= DIFF_FORMAT_DIFFSTAT;
		options->stat_name_width = name_width;
		options->stat_width = width;
	}

	/* renames options */
	else if (!prefixcmp(arg, "-B")) {
		if ((options->break_opt = diff_scoreopt_parse(arg)) == -1)
			return -1;
	}
	else if (!prefixcmp(arg, "-M")) {
		if ((options->rename_score = diff_scoreopt_parse(arg)) == -1)
			return -1;
		options->detect_rename = DIFF_DETECT_RENAME;
	}
	else if (!prefixcmp(arg, "-C")) {
		if (options->detect_rename == DIFF_DETECT_COPY)
			DIFF_OPT_SET(options, FIND_COPIES_HARDER);
		if ((options->rename_score = diff_scoreopt_parse(arg)) == -1)
			return -1;
		options->detect_rename = DIFF_DETECT_COPY;
	}
	else if (!strcmp(arg, "--no-renames"))
		options->detect_rename = 0;
	else if (!strcmp(arg, "--relative"))
		DIFF_OPT_SET(options, RELATIVE_NAME);
	else if (!prefixcmp(arg, "--relative=")) {
		DIFF_OPT_SET(options, RELATIVE_NAME);
		options->prefix = arg + 11;
	}

	/* xdiff options */
	else if (!strcmp(arg, "-w") || !strcmp(arg, "--ignore-all-space"))
		options->xdl_opts |= XDF_IGNORE_WHITESPACE;
	else if (!strcmp(arg, "-b") || !strcmp(arg, "--ignore-space-change"))
		options->xdl_opts |= XDF_IGNORE_WHITESPACE_CHANGE;
	else if (!strcmp(arg, "--ignore-space-at-eol"))
		options->xdl_opts |= XDF_IGNORE_WHITESPACE_AT_EOL;

	/* flags options */
	else if (!strcmp(arg, "--binary")) {
		options->output_format |= DIFF_FORMAT_PATCH;
		DIFF_OPT_SET(options, BINARY);
	}
	else if (!strcmp(arg, "--full-index"))
		DIFF_OPT_SET(options, FULL_INDEX);
	else if (!strcmp(arg, "-a") || !strcmp(arg, "--text"))
		DIFF_OPT_SET(options, TEXT);
	else if (!strcmp(arg, "-R"))
		DIFF_OPT_SET(options, REVERSE_DIFF);
	else if (!strcmp(arg, "--find-copies-harder"))
		DIFF_OPT_SET(options, FIND_COPIES_HARDER);
	else if (!strcmp(arg, "--follow"))
		DIFF_OPT_SET(options, FOLLOW_RENAMES);
	else if (!strcmp(arg, "--color"))
		DIFF_OPT_SET(options, COLOR_DIFF);
	else if (!strcmp(arg, "--no-color"))
		DIFF_OPT_CLR(options, COLOR_DIFF);
	else if (!strcmp(arg, "--color-words"))
		options->flags |= DIFF_OPT_COLOR_DIFF | DIFF_OPT_COLOR_DIFF_WORDS;
	else if (!strcmp(arg, "--exit-code"))
		DIFF_OPT_SET(options, EXIT_WITH_STATUS);
	else if (!strcmp(arg, "--quiet"))
		DIFF_OPT_SET(options, QUIET);
	else if (!strcmp(arg, "--ext-diff"))
		DIFF_OPT_SET(options, ALLOW_EXTERNAL);
	else if (!strcmp(arg, "--no-ext-diff"))
		DIFF_OPT_CLR(options, ALLOW_EXTERNAL);
	else if (!strcmp(arg, "--ignore-submodules"))
		DIFF_OPT_SET(options, IGNORE_SUBMODULES);

	/* misc options */
	else if (!strcmp(arg, "-z"))
		options->line_termination = 0;
	else if (!prefixcmp(arg, "-l"))
		options->rename_limit = strtoul(arg+2, NULL, 10);
	else if (!prefixcmp(arg, "-S"))
		options->pickaxe = arg + 2;
	else if (!strcmp(arg, "--pickaxe-all"))
		options->pickaxe_opts = DIFF_PICKAXE_ALL;
	else if (!strcmp(arg, "--pickaxe-regex"))
		options->pickaxe_opts = DIFF_PICKAXE_REGEX;
	else if (!prefixcmp(arg, "-O"))
		options->orderfile = arg + 2;
	else if (!prefixcmp(arg, "--diff-filter="))
		options->filter = arg + 14;
	else if (!strcmp(arg, "--abbrev"))
		options->abbrev = DEFAULT_ABBREV;
	else if (!prefixcmp(arg, "--abbrev=")) {
		options->abbrev = strtoul(arg + 9, NULL, 10);
		if (options->abbrev < MINIMUM_ABBREV)
			options->abbrev = MINIMUM_ABBREV;
		else if (40 < options->abbrev)
			options->abbrev = 40;
	}
	else if (!prefixcmp(arg, "--src-prefix="))
		options->a_prefix = arg + 13;
	else if (!prefixcmp(arg, "--dst-prefix="))
		options->b_prefix = arg + 13;
	else if (!strcmp(arg, "--no-prefix"))
		options->a_prefix = options->b_prefix = "";
	else if (!prefixcmp(arg, "--output=")) {
		options->file = fopen(arg + strlen("--output="), "w");
		options->close_file = 1;
	} else
		return 0;
	return 1;
}