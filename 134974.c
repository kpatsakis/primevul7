SV *
Perl_re_intuit_string(pTHX_ REGEXP * const r)
{				/* Assume that RE_INTUIT is set */
    struct regexp *const prog = ReANY(r);
    GET_RE_DEBUG_FLAGS_DECL;

    PERL_ARGS_ASSERT_RE_INTUIT_STRING;
    PERL_UNUSED_CONTEXT;

    DEBUG_COMPILE_r(
	{
	    const char * const s = SvPV_nolen_const(RX_UTF8(r)
		      ? prog->check_utf8 : prog->check_substr);

	    if (!PL_colorset) reginitcolors();
            Perl_re_printf( aTHX_
		      "%sUsing REx %ssubstr:%s \"%s%.60s%s%s\"\n",
		      PL_colors[4],
		      RX_UTF8(r) ? "utf8 " : "",
		      PL_colors[5], PL_colors[0],
		      s,
		      PL_colors[1],
		      (strlen(s) > PL_dump_re_max_len ? "..." : ""));
	} );

    /* use UTF8 check substring if regexp pattern itself is in UTF8 */
    return RX_UTF8(r) ? prog->check_utf8 : prog->check_substr;