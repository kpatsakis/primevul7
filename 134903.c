void
Perl_save_re_context(pTHX)
{
    I32 nparens = -1;
    I32 i;

    /* Save $1..$n (#18107: UTF-8 s/(\w+)/uc($1)/e); AMS 20021106. */

    if (PL_curpm) {
	const REGEXP * const rx = PM_GETRE(PL_curpm);
	if (rx)
            nparens = RX_NPARENS(rx);
    }

    /* RT #124109. This is a complete hack; in the SWASHNEW case we know
     * that PL_curpm will be null, but that utf8.pm and the modules it
     * loads will only use $1..$3.
     * The t/porting/re_context.t test file checks this assumption.
     */
    if (nparens == -1)
        nparens = 3;

    for (i = 1; i <= nparens; i++) {
        char digits[TYPE_CHARS(long)];
        const STRLEN len = my_snprintf(digits, sizeof(digits),
                                       "%lu", (long)i);
        GV *const *const gvp
            = (GV**)hv_fetch(PL_defstash, digits, len, 0);

        if (gvp) {
            GV * const gv = *gvp;
            if (SvTYPE(gv) == SVt_PVGV && GvSV(gv))
                save_scalar(gv);
        }
    }