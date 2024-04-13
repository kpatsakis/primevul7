rb_reg_match_m_p(int argc, VALUE *argv, VALUE re)
{
    VALUE str, initpos;
    long pos = 0;
    regex_t *reg;
    onig_errmsg_buffer err = "";
    OnigPosition result;
    const UChar *start, *end;
    int tmpreg;

    rb_scan_args(argc, argv, "11", &str, &initpos);
    if (NIL_P(str)) return Qfalse;
    str = SYMBOL_P(str) ? rb_sym2str(str) : rb_str_to_str(str);
    if (argc == 2) {
	pos = NUM2LONG(initpos);
	if (pos < 0) {
	    pos += NUM2LONG(rb_str_length(str));
	    if (pos < 0) return Qfalse;
	}
	if (pos > 0) {
	    long len = 0;
	    char *beg = rb_str_subpos(str, pos, &len);
	    if (!beg) return Qfalse;
	    pos = beg - RSTRING_PTR(str);
	}
    }
    reg = rb_reg_prepare_re0(re, str, err);
    tmpreg = reg != RREGEXP_PTR(re);
    if (!tmpreg) RREGEXP(re)->usecnt++;
    start = ((UChar*)RSTRING_PTR(str));
    end = start + RSTRING_LEN(str);
    result = onig_search(reg, start, end, start + pos, end,
			 NULL, ONIG_OPTION_NONE);
    if (!tmpreg) RREGEXP(re)->usecnt--;
    if (tmpreg) {
	if (RREGEXP(re)->usecnt) {
	    onig_free(reg);
	}
	else {
	    onig_free(RREGEXP_PTR(re));
	    RREGEXP_PTR(re) = reg;
	}
    }
    if (result < 0) {
	if (result == ONIG_MISMATCH) {
	    return Qfalse;
	}
	else {
	    onig_error_code_to_str((UChar*)err, (int)result);
	    rb_reg_raise(RREGEXP_SRC_PTR(re), RREGEXP_SRC_LEN(re), err, re);
	}
    }
    return Qtrue;
}