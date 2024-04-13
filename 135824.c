static void builtin_diffstat(const char *name_a, const char *name_b,
			     struct diff_filespec *one,
			     struct diff_filespec *two,
			     struct diffstat_t *diffstat,
			     struct diff_options *o,
			     int complete_rewrite)
{
	mmfile_t mf1, mf2;
	struct diffstat_file *data;

	data = diffstat_add(diffstat, name_a, name_b);

	if (!one || !two) {
		data->is_unmerged = 1;
		return;
	}
	if (complete_rewrite) {
		diff_populate_filespec(one, 0);
		diff_populate_filespec(two, 0);
		data->deleted = count_lines(one->data, one->size);
		data->added = count_lines(two->data, two->size);
		goto free_and_return;
	}
	if (fill_mmfile(&mf1, one) < 0 || fill_mmfile(&mf2, two) < 0)
		die("unable to read files to diff");

	if (diff_filespec_is_binary(one) || diff_filespec_is_binary(two)) {
		data->is_binary = 1;
		data->added = mf2.size;
		data->deleted = mf1.size;
	} else {
		/* Crazy xdl interfaces.. */
		xpparam_t xpp;
		xdemitconf_t xecfg;
		xdemitcb_t ecb;

		memset(&xecfg, 0, sizeof(xecfg));
		xpp.flags = XDF_NEED_MINIMAL | o->xdl_opts;
		ecb.outf = xdiff_outf;
		ecb.priv = diffstat;
		xdi_diff(&mf1, &mf2, &xpp, &xecfg, &ecb);
	}

 free_and_return:
	diff_free_filespec_data(one);
	diff_free_filespec_data(two);
}