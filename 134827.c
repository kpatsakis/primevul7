void
Perl_regdump(pTHX_ const regexp *r)
{
#ifdef DEBUGGING
    int i;
    SV * const sv = sv_newmortal();
    SV *dsv= sv_newmortal();
    RXi_GET_DECL(r, ri);
    GET_RE_DEBUG_FLAGS_DECL;

    PERL_ARGS_ASSERT_REGDUMP;

    (void)dumpuntil(r, ri->program, ri->program + 1, NULL, NULL, sv, 0, 0);

    /* Header fields of interest. */
    for (i = 0; i < 2; i++) {
        if (r->substrs->data[i].substr) {
            RE_PV_QUOTED_DECL(s, 0, dsv,
                            SvPVX_const(r->substrs->data[i].substr),
                            RE_SV_DUMPLEN(r->substrs->data[i].substr),
                            PL_dump_re_max_len);
            Perl_re_printf( aTHX_
                          "%s %s%s at %" IVdf "..%" UVuf " ",
                          i ? "floating" : "anchored",
                          s,
                          RE_SV_TAIL(r->substrs->data[i].substr),
                          (IV)r->substrs->data[i].min_offset,
                          (UV)r->substrs->data[i].max_offset);
        }
        else if (r->substrs->data[i].utf8_substr) {
            RE_PV_QUOTED_DECL(s, 1, dsv,
                            SvPVX_const(r->substrs->data[i].utf8_substr),
                            RE_SV_DUMPLEN(r->substrs->data[i].utf8_substr),
                            30);
            Perl_re_printf( aTHX_
                          "%s utf8 %s%s at %" IVdf "..%" UVuf " ",
                          i ? "floating" : "anchored",
                          s,
                          RE_SV_TAIL(r->substrs->data[i].utf8_substr),
                          (IV)r->substrs->data[i].min_offset,
                          (UV)r->substrs->data[i].max_offset);
        }
    }

    if (r->check_substr || r->check_utf8)
        Perl_re_printf( aTHX_
		      (const char *)
		      (   r->check_substr == r->substrs->data[1].substr
		       && r->check_utf8   == r->substrs->data[1].utf8_substr
		       ? "(checking floating" : "(checking anchored"));
    if (r->intflags & PREGf_NOSCAN)
        Perl_re_printf( aTHX_  " noscan");
    if (r->extflags & RXf_CHECK_ALL)
        Perl_re_printf( aTHX_  " isall");
    if (r->check_substr || r->check_utf8)
        Perl_re_printf( aTHX_  ") ");

    if (ri->regstclass) {
        regprop(r, sv, ri->regstclass, NULL, NULL);
        Perl_re_printf( aTHX_  "stclass %s ", SvPVX_const(sv));
    }
    if (r->intflags & PREGf_ANCH) {
        Perl_re_printf( aTHX_  "anchored");
        if (r->intflags & PREGf_ANCH_MBOL)
            Perl_re_printf( aTHX_  "(MBOL)");
        if (r->intflags & PREGf_ANCH_SBOL)
            Perl_re_printf( aTHX_  "(SBOL)");
        if (r->intflags & PREGf_ANCH_GPOS)
            Perl_re_printf( aTHX_  "(GPOS)");
        Perl_re_printf( aTHX_ " ");
    }
    if (r->intflags & PREGf_GPOS_SEEN)
        Perl_re_printf( aTHX_  "GPOS:%" UVuf " ", (UV)r->gofs);
    if (r->intflags & PREGf_SKIP)
        Perl_re_printf( aTHX_  "plus ");
    if (r->intflags & PREGf_IMPLICIT)
        Perl_re_printf( aTHX_  "implicit ");
    Perl_re_printf( aTHX_  "minlen %" IVdf " ", (IV)r->minlen);
    if (r->extflags & RXf_EVAL_SEEN)
        Perl_re_printf( aTHX_  "with eval ");
    Perl_re_printf( aTHX_  "\n");
    DEBUG_FLAGS_r({
        regdump_extflags("r->extflags: ", r->extflags);
        regdump_intflags("r->intflags: ", r->intflags);
    });
#else
    PERL_ARGS_ASSERT_REGDUMP;
    PERL_UNUSED_CONTEXT;
    PERL_UNUSED_ARG(r);
#endif	/* DEBUGGING */