STATIC SV*
S__make_exactf_invlist(pTHX_ RExC_state_t *pRExC_state, regnode *node)
{
    dVAR;
    const U8 * s = (U8*)STRING(node);
    SSize_t bytelen = STR_LEN(node);
    UV uc;
    /* Start out big enough for 2 separate code points */
    SV* invlist = _new_invlist(4);

    PERL_ARGS_ASSERT__MAKE_EXACTF_INVLIST;

    if (! UTF) {
        uc = *s;

        /* We punt and assume can match anything if the node begins
         * with a multi-character fold.  Things are complicated.  For
         * example, /ffi/i could match any of:
         *  "\N{LATIN SMALL LIGATURE FFI}"
         *  "\N{LATIN SMALL LIGATURE FF}I"
         *  "F\N{LATIN SMALL LIGATURE FI}"
         *  plus several other things; and making sure we have all the
         *  possibilities is hard. */
        if (is_MULTI_CHAR_FOLD_latin1_safe(s, s + bytelen)) {
            invlist = _add_range_to_invlist(invlist, 0, UV_MAX);
        }
        else {
            /* Any Latin1 range character can potentially match any
             * other depending on the locale, and in Turkic locales, U+130 and
             * U+131 */
            if (OP(node) == EXACTFL) {
                _invlist_union(invlist, PL_Latin1, &invlist);
                invlist = add_cp_to_invlist(invlist,
                                                LATIN_SMALL_LETTER_DOTLESS_I);
                invlist = add_cp_to_invlist(invlist,
                                        LATIN_CAPITAL_LETTER_I_WITH_DOT_ABOVE);
            }
            else {
                /* But otherwise, it matches at least itself.  We can
                 * quickly tell if it has a distinct fold, and if so,
                 * it matches that as well */
                invlist = add_cp_to_invlist(invlist, uc);
                if (IS_IN_SOME_FOLD_L1(uc))
                    invlist = add_cp_to_invlist(invlist, PL_fold_latin1[uc]);
            }

            /* Some characters match above-Latin1 ones under /i.  This
             * is true of EXACTFL ones when the locale is UTF-8 */
            if (HAS_NONLATIN1_SIMPLE_FOLD_CLOSURE(uc)
                && (! isASCII(uc) || (OP(node) != EXACTFAA
                                    && OP(node) != EXACTFAA_NO_TRIE)))
            {
                add_above_Latin1_folds(pRExC_state, (U8) uc, &invlist);
            }
        }
    }
    else {  /* Pattern is UTF-8 */
        U8 folded[UTF8_MAX_FOLD_CHAR_EXPAND * UTF8_MAXBYTES_CASE + 1] = { '\0' };
        const U8* e = s + bytelen;
        IV fc;

        fc = uc = utf8_to_uvchr_buf(s, s + bytelen, NULL);

        /* The only code points that aren't folded in a UTF EXACTFish
         * node are are the problematic ones in EXACTFL nodes */
        if (OP(node) == EXACTFL && is_PROBLEMATIC_LOCALE_FOLDEDS_START_cp(uc)) {
            /* We need to check for the possibility that this EXACTFL
             * node begins with a multi-char fold.  Therefore we fold
             * the first few characters of it so that we can make that
             * check */
            U8 *d = folded;
            int i;

            fc = -1;
            for (i = 0; i < UTF8_MAX_FOLD_CHAR_EXPAND && s < e; i++) {
                if (isASCII(*s)) {
                    *(d++) = (U8) toFOLD(*s);
                    if (fc < 0) {       /* Save the first fold */
                        fc = *(d-1);
                    }
                    s++;
                }
                else {
                    STRLEN len;
                    UV fold = toFOLD_utf8_safe(s, e, d, &len);
                    if (fc < 0) {       /* Save the first fold */
                        fc = fold;
                    }
                    d += len;
                    s += UTF8SKIP(s);
                }
            }

            /* And set up so the code below that looks in this folded
             * buffer instead of the node's string */
            e = d;
            s = folded;
        }

        /* When we reach here 's' points to the fold of the first
         * character(s) of the node; and 'e' points to far enough along
         * the folded string to be just past any possible multi-char
         * fold.
         *
         * Unlike the non-UTF-8 case, the macro for determining if a
         * string is a multi-char fold requires all the characters to
         * already be folded.  This is because of all the complications
         * if not.  Note that they are folded anyway, except in EXACTFL
         * nodes.  Like the non-UTF case above, we punt if the node
         * begins with a multi-char fold  */

        if (is_MULTI_CHAR_FOLD_utf8_safe(s, e)) {
            invlist = _add_range_to_invlist(invlist, 0, UV_MAX);
        }
        else {  /* Single char fold */
            unsigned int k;
            unsigned int first_fold;
            const unsigned int * remaining_folds;
            Size_t folds_count;

            /* It matches itself */
            invlist = add_cp_to_invlist(invlist, fc);

            /* ... plus all the things that fold to it, which are found in
             * PL_utf8_foldclosures */
            folds_count = _inverse_folds(fc, &first_fold,
                                                &remaining_folds);
            for (k = 0; k < folds_count; k++) {
                UV c = (k == 0) ? first_fold : remaining_folds[k-1];

                /* /aa doesn't allow folds between ASCII and non- */
                if (   (OP(node) == EXACTFAA || OP(node) == EXACTFAA_NO_TRIE)
                    && isASCII(c) != isASCII(fc))
                {
                    continue;
                }

                invlist = add_cp_to_invlist(invlist, c);
            }

            if (OP(node) == EXACTFL) {

                /* If either [iI] are present in an EXACTFL node the above code
                 * should have added its normal case pair, but under a Turkish
                 * locale they could match instead the case pairs from it.  Add
                 * those as potential matches as well */
                if (isALPHA_FOLD_EQ(fc, 'I')) {
                    invlist = add_cp_to_invlist(invlist,
                                                LATIN_SMALL_LETTER_DOTLESS_I);
                    invlist = add_cp_to_invlist(invlist,
                                        LATIN_CAPITAL_LETTER_I_WITH_DOT_ABOVE);
                }
                else if (fc == LATIN_SMALL_LETTER_DOTLESS_I) {
                    invlist = add_cp_to_invlist(invlist, 'I');
                }
                else if (fc == LATIN_CAPITAL_LETTER_I_WITH_DOT_ABOVE) {
                    invlist = add_cp_to_invlist(invlist, 'i');
                }
            }
        }
    }

    return invlist;