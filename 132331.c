input_textarea(FormItemList *fi)
{
    char *tmpf = tmpfname(TMPF_DFL, NULL)->ptr;
    Str tmp;
    FILE *f;
#ifdef USE_M17N
    wc_ces charset = DisplayCharset;
    wc_uint8 auto_detect;
#endif

    f = fopen(tmpf, "w");
    if (f == NULL) {
	/* FIXME: gettextize? */
	disp_err_message("Can't open temporary file", FALSE);
	return;
    }
    if (fi->value)
	form_fputs_decode(fi->value, f);
    fclose(f);

    fmTerm();
    system(myEditor(Editor, tmpf, 1)->ptr);
    fmInit();

    if (fi->readonly)
	goto input_end;
    f = fopen(tmpf, "r");
    if (f == NULL) {
	/* FIXME: gettextize? */
	disp_err_message("Can't open temporary file", FALSE);
	goto input_end;
    }
    fi->value = Strnew();
#ifdef USE_M17N
    auto_detect = WcOption.auto_detect;
    WcOption.auto_detect = WC_OPT_DETECT_ON;
#endif
    while (tmp = Strfgets(f), tmp->length > 0) {
	if (tmp->length == 1 && tmp->ptr[tmp->length - 1] == '\n') {
	    /* null line with bare LF */
	    tmp = Strnew_charp("\r\n");
	}
	else if (tmp->length > 1 && tmp->ptr[tmp->length - 1] == '\n' &&
		 tmp->ptr[tmp->length - 2] != '\r') {
	    Strshrink(tmp, 1);
	    Strcat_charp(tmp, "\r\n");
	}
	tmp = convertLine(NULL, tmp, RAW_MODE, &charset, DisplayCharset);
	Strcat(fi->value, tmp);
    }
#ifdef USE_M17N
    WcOption.auto_detect = auto_detect;
#endif
    fclose(f);
  input_end:
    unlink(tmpf);
}