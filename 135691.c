static void builtin_checkdiff(const char *name_a, const char *name_b,
			      const char *attr_path,
			     struct diff_filespec *one,
			     struct diff_filespec *two, struct diff_options *o)
{
	mmfile_t mf1, mf2;
	struct checkdiff_t data;

	if (!two)
		return;

	memset(&data, 0, sizeof(data));
	data.xm.consume = checkdiff_consume;
	data.filename = name_b ? name_b : name_a;
	data.lineno = 0;
	data.color_diff = DIFF_OPT_TST(o, COLOR_DIFF);
	data.ws_rule = whitespace_rule(attr_path);
	data.file = o->file;

	if (fill_mmfile(&mf1, one) < 0 || fill_mmfile(&mf2, two) < 0)
		die("unable to read files to diff");

	if (diff_filespec_is_binary(two))
		goto free_and_return;
	else {
		/* Crazy xdl interfaces.. */
		xpparam_t xpp;
		xdemitconf_t xecfg;
		xdemitcb_t ecb;

		memset(&xecfg, 0, sizeof(xecfg));
		xpp.flags = XDF_NEED_MINIMAL;
		ecb.outf = xdiff_outf;
		ecb.priv = &data;
		xdi_diff(&mf1, &mf2, &xpp, &xecfg, &ecb);
	}
 free_and_return:
	diff_free_filespec_data(one);
	diff_free_filespec_data(two);
	if (data.status)
		DIFF_OPT_SET(o, CHECK_FAILED);
}