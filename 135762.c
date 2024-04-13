static void run_diff(struct diff_filepair *p, struct diff_options *o)
{
	const char *pgm = external_diff();
	struct strbuf msg;
	char *xfrm_msg;
	struct diff_filespec *one = p->one;
	struct diff_filespec *two = p->two;
	const char *name;
	const char *other;
	const char *attr_path;
	int complete_rewrite = 0;

	name  = p->one->path;
	other = (strcmp(name, p->two->path) ? p->two->path : NULL);
	attr_path = name;
	if (o->prefix_length)
		strip_prefix(o->prefix_length, &name, &other);

	if (DIFF_PAIR_UNMERGED(p)) {
		run_diff_cmd(pgm, name, NULL, attr_path,
			     NULL, NULL, NULL, o, 0);
		return;
	}

	diff_fill_sha1_info(one);
	diff_fill_sha1_info(two);

	strbuf_init(&msg, PATH_MAX * 2 + 300);
	switch (p->status) {
	case DIFF_STATUS_COPIED:
		strbuf_addf(&msg, "similarity index %d%%", similarity_index(p));
		strbuf_addstr(&msg, "\ncopy from ");
		quote_c_style(name, &msg, NULL, 0);
		strbuf_addstr(&msg, "\ncopy to ");
		quote_c_style(other, &msg, NULL, 0);
		strbuf_addch(&msg, '\n');
		break;
	case DIFF_STATUS_RENAMED:
		strbuf_addf(&msg, "similarity index %d%%", similarity_index(p));
		strbuf_addstr(&msg, "\nrename from ");
		quote_c_style(name, &msg, NULL, 0);
		strbuf_addstr(&msg, "\nrename to ");
		quote_c_style(other, &msg, NULL, 0);
		strbuf_addch(&msg, '\n');
		break;
	case DIFF_STATUS_MODIFIED:
		if (p->score) {
			strbuf_addf(&msg, "dissimilarity index %d%%\n",
					similarity_index(p));
			complete_rewrite = 1;
			break;
		}
		/* fallthru */
	default:
		/* nothing */
		;
	}

	if (hashcmp(one->sha1, two->sha1)) {
		int abbrev = DIFF_OPT_TST(o, FULL_INDEX) ? 40 : DEFAULT_ABBREV;

		if (DIFF_OPT_TST(o, BINARY)) {
			mmfile_t mf;
			if ((!fill_mmfile(&mf, one) && diff_filespec_is_binary(one)) ||
			    (!fill_mmfile(&mf, two) && diff_filespec_is_binary(two)))
				abbrev = 40;
		}
		strbuf_addf(&msg, "index %.*s..%.*s",
				abbrev, sha1_to_hex(one->sha1),
				abbrev, sha1_to_hex(two->sha1));
		if (one->mode == two->mode)
			strbuf_addf(&msg, " %06o", one->mode);
		strbuf_addch(&msg, '\n');
	}

	if (msg.len)
		strbuf_setlen(&msg, msg.len - 1);
	xfrm_msg = msg.len ? msg.buf : NULL;

	if (!pgm &&
	    DIFF_FILE_VALID(one) && DIFF_FILE_VALID(two) &&
	    (S_IFMT & one->mode) != (S_IFMT & two->mode)) {
		/* a filepair that changes between file and symlink
		 * needs to be split into deletion and creation.
		 */
		struct diff_filespec *null = alloc_filespec(two->path);
		run_diff_cmd(NULL, name, other, attr_path,
			     one, null, xfrm_msg, o, 0);
		free(null);
		null = alloc_filespec(one->path);
		run_diff_cmd(NULL, name, other, attr_path,
			     null, two, xfrm_msg, o, 0);
		free(null);
	}
	else
		run_diff_cmd(pgm, name, other, attr_path,
			     one, two, xfrm_msg, o, complete_rewrite);

	strbuf_release(&msg);
}