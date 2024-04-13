int setup_revisions(int argc, const char **argv, struct rev_info *revs, const char *def)
{
	int i, flags, seen_dashdash, show_merge;
	const char **unrecognized = argv + 1;
	int left = 1;
	int all_match = 0;
	int regflags = 0;
	int fixed = 0;

	/* First, search for "--" */
	seen_dashdash = 0;
	for (i = 1; i < argc; i++) {
		const char *arg = argv[i];
		if (strcmp(arg, "--"))
			continue;
		argv[i] = NULL;
		argc = i;
		if (argv[i + 1])
			revs->prune_data = get_pathspec(revs->prefix, argv + i + 1);
		seen_dashdash = 1;
		break;
	}

	flags = show_merge = 0;
	for (i = 1; i < argc; i++) {
		const char *arg = argv[i];
		if (*arg == '-') {
			int opts;
			if (!prefixcmp(arg, "--max-count=")) {
				revs->max_count = atoi(arg + 12);
				continue;
			}
			if (!prefixcmp(arg, "--skip=")) {
				revs->skip_count = atoi(arg + 7);
				continue;
			}
			/* accept -<digit>, like traditional "head" */
			if ((*arg == '-') && isdigit(arg[1])) {
				revs->max_count = atoi(arg + 1);
				continue;
			}
			if (!strcmp(arg, "-n")) {
				if (argc <= i + 1)
					die("-n requires an argument");
				revs->max_count = atoi(argv[++i]);
				continue;
			}
			if (!prefixcmp(arg, "-n")) {
				revs->max_count = atoi(arg + 2);
				continue;
			}
			if (!prefixcmp(arg, "--max-age=")) {
				revs->max_age = atoi(arg + 10);
				continue;
			}
			if (!prefixcmp(arg, "--since=")) {
				revs->max_age = approxidate(arg + 8);
				continue;
			}
			if (!prefixcmp(arg, "--after=")) {
				revs->max_age = approxidate(arg + 8);
				continue;
			}
			if (!prefixcmp(arg, "--min-age=")) {
				revs->min_age = atoi(arg + 10);
				continue;
			}
			if (!prefixcmp(arg, "--before=")) {
				revs->min_age = approxidate(arg + 9);
				continue;
			}
			if (!prefixcmp(arg, "--until=")) {
				revs->min_age = approxidate(arg + 8);
				continue;
			}
			if (!strcmp(arg, "--all")) {
				handle_refs(revs, flags, for_each_ref);
				continue;
			}
			if (!strcmp(arg, "--branches")) {
				handle_refs(revs, flags, for_each_branch_ref);
				continue;
			}
			if (!strcmp(arg, "--tags")) {
				handle_refs(revs, flags, for_each_tag_ref);
				continue;
			}
			if (!strcmp(arg, "--remotes")) {
				handle_refs(revs, flags, for_each_remote_ref);
				continue;
			}
			if (!strcmp(arg, "--first-parent")) {
				revs->first_parent_only = 1;
				continue;
			}
			if (!strcmp(arg, "--reflog")) {
				handle_reflog(revs, flags);
				continue;
			}
			if (!strcmp(arg, "-g") ||
					!strcmp(arg, "--walk-reflogs")) {
				init_reflog_walk(&revs->reflog_info);
				continue;
			}
			if (!strcmp(arg, "--not")) {
				flags ^= UNINTERESTING;
				continue;
			}
			if (!strcmp(arg, "--default")) {
				if (++i >= argc)
					die("bad --default argument");
				def = argv[i];
				continue;
			}
			if (!strcmp(arg, "--merge")) {
				show_merge = 1;
				continue;
			}
			if (!strcmp(arg, "--topo-order")) {
				revs->lifo = 1;
				revs->topo_order = 1;
				continue;
			}
			if (!strcmp(arg, "--date-order")) {
				revs->lifo = 0;
				revs->topo_order = 1;
				continue;
			}
			if (!prefixcmp(arg, "--early-output")) {
				int count = 100;
				switch (arg[14]) {
				case '=':
					count = atoi(arg+15);
					/* Fallthrough */
				case 0:
					revs->topo_order = 1;
					revs->early_output = count;
					continue;
				}
			}
			if (!strcmp(arg, "--parents")) {
				revs->rewrite_parents = 1;
				revs->print_parents = 1;
				continue;
			}
			if (!strcmp(arg, "--dense")) {
				revs->dense = 1;
				continue;
			}
			if (!strcmp(arg, "--sparse")) {
				revs->dense = 0;
				continue;
			}
			if (!strcmp(arg, "--show-all")) {
				revs->show_all = 1;
				continue;
			}
			if (!strcmp(arg, "--remove-empty")) {
				revs->remove_empty_trees = 1;
				continue;
			}
			if (!strcmp(arg, "--no-merges")) {
				revs->no_merges = 1;
				continue;
			}
			if (!strcmp(arg, "--boundary")) {
				revs->boundary = 1;
				continue;
			}
			if (!strcmp(arg, "--left-right")) {
				revs->left_right = 1;
				continue;
			}
			if (!strcmp(arg, "--cherry-pick")) {
				revs->cherry_pick = 1;
				revs->limited = 1;
				continue;
			}
			if (!strcmp(arg, "--objects")) {
				revs->tag_objects = 1;
				revs->tree_objects = 1;
				revs->blob_objects = 1;
				continue;
			}
			if (!strcmp(arg, "--objects-edge")) {
				revs->tag_objects = 1;
				revs->tree_objects = 1;
				revs->blob_objects = 1;
				revs->edge_hint = 1;
				continue;
			}
			if (!strcmp(arg, "--unpacked")) {
				revs->unpacked = 1;
				free(revs->ignore_packed);
				revs->ignore_packed = NULL;
				revs->num_ignore_packed = 0;
				continue;
			}
			if (!prefixcmp(arg, "--unpacked=")) {
				revs->unpacked = 1;
				add_ignore_packed(revs, arg+11);
				continue;
			}
			if (!strcmp(arg, "-r")) {
				revs->diff = 1;
				DIFF_OPT_SET(&revs->diffopt, RECURSIVE);
				continue;
			}
			if (!strcmp(arg, "-t")) {
				revs->diff = 1;
				DIFF_OPT_SET(&revs->diffopt, RECURSIVE);
				DIFF_OPT_SET(&revs->diffopt, TREE_IN_RECURSIVE);
				continue;
			}
			if (!strcmp(arg, "-m")) {
				revs->ignore_merges = 0;
				continue;
			}
			if (!strcmp(arg, "-c")) {
				revs->diff = 1;
				revs->dense_combined_merges = 0;
				revs->combine_merges = 1;
				continue;
			}
			if (!strcmp(arg, "--cc")) {
				revs->diff = 1;
				revs->dense_combined_merges = 1;
				revs->combine_merges = 1;
				continue;
			}
			if (!strcmp(arg, "-v")) {
				revs->verbose_header = 1;
				continue;
			}
			if (!strcmp(arg, "--pretty")) {
				revs->verbose_header = 1;
				get_commit_format(arg+8, revs);
				continue;
			}
			if (!prefixcmp(arg, "--pretty=")) {
				revs->verbose_header = 1;
				get_commit_format(arg+9, revs);
				continue;
			}
			if (!strcmp(arg, "--graph")) {
				revs->topo_order = 1;
				revs->rewrite_parents = 1;
				revs->graph = graph_init(revs);
				continue;
			}
			if (!strcmp(arg, "--root")) {
				revs->show_root_diff = 1;
				continue;
			}
			if (!strcmp(arg, "--no-commit-id")) {
				revs->no_commit_id = 1;
				continue;
			}
			if (!strcmp(arg, "--always")) {
				revs->always_show_header = 1;
				continue;
			}
			if (!strcmp(arg, "--no-abbrev")) {
				revs->abbrev = 0;
				continue;
			}
			if (!strcmp(arg, "--abbrev")) {
				revs->abbrev = DEFAULT_ABBREV;
				continue;
			}
			if (!prefixcmp(arg, "--abbrev=")) {
				revs->abbrev = strtoul(arg + 9, NULL, 10);
				if (revs->abbrev < MINIMUM_ABBREV)
					revs->abbrev = MINIMUM_ABBREV;
				else if (revs->abbrev > 40)
					revs->abbrev = 40;
				continue;
			}
			if (!strcmp(arg, "--abbrev-commit")) {
				revs->abbrev_commit = 1;
				continue;
			}
			if (!strcmp(arg, "--full-diff")) {
				revs->diff = 1;
				revs->full_diff = 1;
				continue;
			}
			if (!strcmp(arg, "--full-history")) {
				revs->simplify_history = 0;
				continue;
			}
			if (!strcmp(arg, "--relative-date")) {
				revs->date_mode = DATE_RELATIVE;
				continue;
			}
			if (!strncmp(arg, "--date=", 7)) {
				revs->date_mode = parse_date_format(arg + 7);
				continue;
			}
			if (!strcmp(arg, "--log-size")) {
				revs->show_log_size = 1;
				continue;
			}

			/*
			 * Grepping the commit log
			 */
			if (!prefixcmp(arg, "--author=")) {
				add_header_grep(revs, "author", arg+9);
				continue;
			}
			if (!prefixcmp(arg, "--committer=")) {
				add_header_grep(revs, "committer", arg+12);
				continue;
			}
			if (!prefixcmp(arg, "--grep=")) {
				add_message_grep(revs, arg+7);
				continue;
			}
			if (!strcmp(arg, "--extended-regexp") ||
			    !strcmp(arg, "-E")) {
				regflags |= REG_EXTENDED;
				continue;
			}
			if (!strcmp(arg, "--regexp-ignore-case") ||
			    !strcmp(arg, "-i")) {
				regflags |= REG_ICASE;
				continue;
			}
			if (!strcmp(arg, "--fixed-strings") ||
			    !strcmp(arg, "-F")) {
				fixed = 1;
				continue;
			}
			if (!strcmp(arg, "--all-match")) {
				all_match = 1;
				continue;
			}
			if (!prefixcmp(arg, "--encoding=")) {
				arg += 11;
				if (strcmp(arg, "none"))
					git_log_output_encoding = xstrdup(arg);
				else
					git_log_output_encoding = "";
				continue;
			}
			if (!strcmp(arg, "--reverse")) {
				revs->reverse ^= 1;
				continue;
			}
			if (!strcmp(arg, "--no-walk")) {
				revs->no_walk = 1;
				continue;
			}
			if (!strcmp(arg, "--do-walk")) {
				revs->no_walk = 0;
				continue;
			}

			opts = diff_opt_parse(&revs->diffopt, argv+i, argc-i);
			if (opts > 0) {
				i += opts - 1;
				continue;
			}
			*unrecognized++ = arg;
			left++;
			continue;
		}

		if (handle_revision_arg(arg, revs, flags, seen_dashdash)) {
			int j;
			if (seen_dashdash || *arg == '^')
				die("bad revision '%s'", arg);

			/* If we didn't have a "--":
			 * (1) all filenames must exist;
			 * (2) all rev-args must not be interpretable
			 *     as a valid filename.
			 * but the latter we have checked in the main loop.
			 */
			for (j = i; j < argc; j++)
				verify_filename(revs->prefix, argv[j]);

			revs->prune_data = get_pathspec(revs->prefix,
							argv + i);
			break;
		}
	}

	if (revs->grep_filter) {
		revs->grep_filter->regflags |= regflags;
		revs->grep_filter->fixed = fixed;
	}

	if (show_merge)
		prepare_show_merge(revs);
	if (def && !revs->pending.nr) {
		unsigned char sha1[20];
		struct object *object;
		unsigned mode;
		if (get_sha1_with_mode(def, sha1, &mode))
			die("bad default revision '%s'", def);
		object = get_reference(revs, def, sha1, 0);
		add_pending_object_with_mode(revs, object, def, mode);
	}

	/* Did the user ask for any diff output? Run the diff! */
	if (revs->diffopt.output_format & ~DIFF_FORMAT_NO_OUTPUT)
		revs->diff = 1;

	/* Pickaxe, diff-filter and rename following need diffs */
	if (revs->diffopt.pickaxe ||
	    revs->diffopt.filter ||
	    DIFF_OPT_TST(&revs->diffopt, FOLLOW_RENAMES))
		revs->diff = 1;

	if (revs->topo_order)
		revs->limited = 1;

	if (revs->prune_data) {
		diff_tree_setup_paths(revs->prune_data, &revs->pruning);
		/* Can't prune commits with rename following: the paths change.. */
		if (!DIFF_OPT_TST(&revs->diffopt, FOLLOW_RENAMES))
			revs->prune = 1;
		if (!revs->full_diff)
			diff_tree_setup_paths(revs->prune_data, &revs->diffopt);
	}
	if (revs->combine_merges) {
		revs->ignore_merges = 0;
		if (revs->dense_combined_merges && !revs->diffopt.output_format)
			revs->diffopt.output_format = DIFF_FORMAT_PATCH;
	}
	revs->diffopt.abbrev = revs->abbrev;
	if (diff_setup_done(&revs->diffopt) < 0)
		die("diff_setup_done failed");

	if (revs->grep_filter) {
		revs->grep_filter->all_match = all_match;
		compile_grep_patterns(revs->grep_filter);
	}

	if (revs->reverse && revs->reflog_info)
		die("cannot combine --reverse with --walk-reflogs");

	/*
	 * Limitations on the graph functionality
	 */
	if (revs->reverse && revs->graph)
		die("cannot combine --reverse with --graph");

	if (revs->reflog_info && revs->graph)
		die("cannot combine --walk-reflogs with --graph");

	return left;
}