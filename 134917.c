STATIC SV*
S_put_charclass_bitmap_innards_common(pTHX_
        SV* invlist,            /* The bitmap */
        SV* posixes,            /* Under /l, things like [:word:], \S */
        SV* only_utf8,          /* Under /d, matches iff the target is UTF-8 */
        SV* not_utf8,           /* /d, matches iff the target isn't UTF-8 */
        SV* only_utf8_locale,   /* Under /l, matches if the locale is UTF-8 */
        const bool invert       /* Is the result to be inverted? */
)
{
    /* Create and return an SV containing a displayable version of the bitmap
     * and associated information determined by the input parameters.  If the
     * output would have been only the inversion indicator '^', NULL is instead
     * returned. */

    dVAR;
    SV * output;

    PERL_ARGS_ASSERT_PUT_CHARCLASS_BITMAP_INNARDS_COMMON;

    if (invert) {
        output = newSVpvs("^");
    }
    else {
        output = newSVpvs("");
    }

    /* First, the code points in the bitmap that are unconditionally there */
    put_charclass_bitmap_innards_invlist(output, invlist);

    /* Traditionally, these have been placed after the main code points */
    if (posixes) {
        sv_catsv(output, posixes);
    }

    if (only_utf8 && _invlist_len(only_utf8)) {
        Perl_sv_catpvf(aTHX_ output, "%s{utf8}%s", PL_colors[1], PL_colors[0]);
        put_charclass_bitmap_innards_invlist(output, only_utf8);
    }

    if (not_utf8 && _invlist_len(not_utf8)) {
        Perl_sv_catpvf(aTHX_ output, "%s{not utf8}%s", PL_colors[1], PL_colors[0]);
        put_charclass_bitmap_innards_invlist(output, not_utf8);
    }

    if (only_utf8_locale && _invlist_len(only_utf8_locale)) {
        Perl_sv_catpvf(aTHX_ output, "%s{utf8 locale}%s", PL_colors[1], PL_colors[0]);
        put_charclass_bitmap_innards_invlist(output, only_utf8_locale);

        /* This is the only list in this routine that can legally contain code
         * points outside the bitmap range.  The call just above to
         * 'put_charclass_bitmap_innards_invlist' will simply suppress them, so
         * output them here.  There's about a half-dozen possible, and none in
         * contiguous ranges longer than 2 */
        if (invlist_highest(only_utf8_locale) >= NUM_ANYOF_CODE_POINTS) {
            UV start, end;
            SV* above_bitmap = NULL;

            _invlist_subtract(only_utf8_locale, PL_InBitmap, &above_bitmap);

            invlist_iterinit(above_bitmap);
            while (invlist_iternext(above_bitmap, &start, &end)) {
                UV i;

                for (i = start; i <= end; i++) {
                    put_code_point(output, i);
                }
            }
            invlist_iterfinish(above_bitmap);
            SvREFCNT_dec_NN(above_bitmap);
        }
    }

    if (invert && SvCUR(output) == 1) {
        return NULL;
    }

    return output;