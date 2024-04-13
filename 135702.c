static void builtin_diff(const char *name_a,
			 const char *name_b,
			 struct diff_filespec *one,
			 struct diff_filespec *two,
			 const char *xfrm_msg,
			 struct diff_options *o,
			 int complete_rewrite)
{
	mmfile_t mf1, mf2;
	const char *lbl[2];
	char *a_one, *b_two;
	const char *set = diff_get_color_opt(o, DIFF_METAINFO);
	const char *reset = diff_get_color_opt(o, DIFF_RESET);

	a_one = quote_two(o->a_prefix, name_a + (*name_a == '/'));
	b_two = quote_two(o->b_prefix, name_b + (*name_b == '/'));
	lbl[0] = DIFF_FILE_VALID(one) ? a_one : "/dev/null";
	lbl[1] = DIFF_FILE_VALID(two) ? b_two : "/dev/null";
	fprintf(o->file, "%sdiff --git %s %s%s\n", set, a_one, b_two, reset);
	if (lbl[0][0] == '/') {
		/* /dev/null */
		fprintf(o->file, "%snew file mode %06o%s\n", set, two->mode, reset);
		if (xfrm_msg && xfrm_msg[0])
			fprintf(o->file, "%s%s%s\n", set, xfrm_msg, reset);
	}
	else if (lbl[1][0] == '/') {
		fprintf(o->file, "%sdeleted file mode %06o%s\n", set, one->mode, reset);
		if (xfrm_msg && xfrm_msg[0])
			fprintf(o->file, "%s%s%s\n", set, xfrm_msg, reset);
	}
	else {
		if (one->mode != two->mode) {
			fprintf(o->file, "%sold mode %06o%s\n", set, one->mode, reset);
			fprintf(o->file, "%snew mode %06o%s\n", set, two->mode, reset);
		}
		if (xfrm_msg && xfrm_msg[0])
			fprintf(o->file, "%s%s%s\n", set, xfrm_msg, reset);
		/*
		 * we do not run diff between different kind
		 * of objects.
		 */
		if ((one->mode ^ two->mode) & S_IFMT)
			goto free_ab_and_return;
		if (complete_rewrite) {
			emit_rewrite_diff(name_a, name_b, one, two, o);
			o->found_changes = 1;
			goto free_ab_and_return;
		}
	}

	if (fill_mmfile(&mf1, one) < 0 || fill_mmfile(&mf2, two) < 0)
		die("unable to read files to diff");

	if (!DIFF_OPT_TST(o, TEXT) &&
	    (diff_filespec_is_binary(one) || diff_filespec_is_binary(two))) {
		/* Quite common confusing case */
		if (mf1.size == mf2.size &&
		    !memcmp(mf1.ptr, mf2.ptr, mf1.size))
			goto free_ab_and_return;
		if (DIFF_OPT_TST(o, BINARY))
			emit_binary_diff(o->file, &mf1, &mf2);
		else
			fprintf(o->file, "Binary files %s and %s differ\n",
				lbl[0], lbl[1]);
		o->found_changes = 1;
	}
	else {
		/* Crazy xdl interfaces.. */
		const char *diffopts = getenv("GIT_DIFF_OPTS");
		xpparam_t xpp;
		xdemitconf_t xecfg;
		xdemitcb_t ecb;
		struct emit_callback ecbdata;
		const char *funcname_pattern;

		funcname_pattern = diff_funcname_pattern(one);
		if (!funcname_pattern)
			funcname_pattern = diff_funcname_pattern(two);

		memset(&xecfg, 0, sizeof(xecfg));
		memset(&ecbdata, 0, sizeof(ecbdata));
		ecbdata.label_path = lbl;
		ecbdata.color_diff = DIFF_OPT_TST(o, COLOR_DIFF);
		ecbdata.found_changesp = &o->found_changes;
		ecbdata.ws_rule = whitespace_rule(name_b ? name_b : name_a);
		ecbdata.file = o->file;
		xpp.flags = XDF_NEED_MINIMAL | o->xdl_opts;
		xecfg.ctxlen = o->context;
		xecfg.flags = XDL_EMIT_FUNCNAMES;
		if (funcname_pattern)
			xdiff_set_find_func(&xecfg, funcname_pattern);
		if (!diffopts)
			;
		else if (!prefixcmp(diffopts, "--unified="))
			xecfg.ctxlen = strtoul(diffopts + 10, NULL, 10);
		else if (!prefixcmp(diffopts, "-u"))
			xecfg.ctxlen = strtoul(diffopts + 2, NULL, 10);
		ecb.outf = xdiff_outf;
		ecb.priv = &ecbdata;
		ecbdata.xm.consume = fn_out_consume;
		if (DIFF_OPT_TST(o, COLOR_DIFF_WORDS)) {
			ecbdata.diff_words =
				xcalloc(1, sizeof(struct diff_words_data));
			ecbdata.diff_words->file = o->file;
		}
		xdi_diff(&mf1, &mf2, &xpp, &xecfg, &ecb);
		if (DIFF_OPT_TST(o, COLOR_DIFF_WORDS))
			free_diff_words_data(&ecbdata);
	}

 free_ab_and_return:
	diff_free_filespec_data(one);
	diff_free_filespec_data(two);
	free(a_one);
	free(b_two);
	return;
}