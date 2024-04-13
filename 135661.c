static void diff_words_show(struct diff_words_data *diff_words)
{
	xpparam_t xpp;
	xdemitconf_t xecfg;
	xdemitcb_t ecb;
	mmfile_t minus, plus;
	int i;

	memset(&xecfg, 0, sizeof(xecfg));
	minus.size = diff_words->minus.text.size;
	minus.ptr = xmalloc(minus.size);
	memcpy(minus.ptr, diff_words->minus.text.ptr, minus.size);
	for (i = 0; i < minus.size; i++)
		if (isspace(minus.ptr[i]))
			minus.ptr[i] = '\n';
	diff_words->minus.current = 0;

	plus.size = diff_words->plus.text.size;
	plus.ptr = xmalloc(plus.size);
	memcpy(plus.ptr, diff_words->plus.text.ptr, plus.size);
	for (i = 0; i < plus.size; i++)
		if (isspace(plus.ptr[i]))
			plus.ptr[i] = '\n';
	diff_words->plus.current = 0;

	xpp.flags = XDF_NEED_MINIMAL;
	xecfg.ctxlen = diff_words->minus.alloc + diff_words->plus.alloc;
	ecb.outf = xdiff_outf;
	ecb.priv = diff_words;
	diff_words->xm.consume = fn_out_diff_words_aux;
	xdi_diff(&minus, &plus, &xpp, &xecfg, &ecb);

	free(minus.ptr);
	free(plus.ptr);
	diff_words->minus.text.size = diff_words->plus.text.size = 0;

	if (diff_words->minus.suppressed_newline) {
		putc('\n', diff_words->file);
		diff_words->minus.suppressed_newline = 0;
	}
}