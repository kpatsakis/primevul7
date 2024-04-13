STATIC bool
S_put_charclass_bitmap_innards(pTHX_ SV *sv,
                                     char *bitmap,
                                     SV *nonbitmap_invlist,
                                     SV *only_utf8_locale_invlist,
                                     const regnode * const node,
                                     const bool force_as_is_display)
{
    /* Appends to 'sv' a displayable version of the innards of the bracketed
     * character class defined by the other arguments:
     *  'bitmap' points to the bitmap, or NULL if to ignore that.
     *  'nonbitmap_invlist' is an inversion list of the code points that are in
     *      the bitmap range, but for some reason aren't in the bitmap; NULL if
     *      none.  The reasons for this could be that they require some
     *      condition such as the target string being or not being in UTF-8
     *      (under /d), or because they came from a user-defined property that
     *      was not resolved at the time of the regex compilation (under /u)
     *  'only_utf8_locale_invlist' is an inversion list of the code points that
     *      are valid only if the runtime locale is a UTF-8 one; NULL if none
     *  'node' is the regex pattern ANYOF node.  It is needed only when the
     *      above two parameters are not null, and is passed so that this
     *      routine can tease apart the various reasons for them.
     *  'force_as_is_display' is TRUE if this routine should definitely NOT try
     *      to invert things to see if that leads to a cleaner display.  If
     *      FALSE, this routine is free to use its judgment about doing this.
     *
     * It returns TRUE if there was actually something output.  (It may be that
     * the bitmap, etc is empty.)
     *
     * When called for outputting the bitmap of a non-ANYOF node, just pass the
     * bitmap, with the succeeding parameters set to NULL, and the final one to
     * FALSE.
     */

    /* In general, it tries to display the 'cleanest' representation of the
     * innards, choosing whether to display them inverted or not, regardless of
     * whether the class itself is to be inverted.  However,  there are some
     * cases where it can't try inverting, as what actually matches isn't known
     * until runtime, and hence the inversion isn't either. */

    dVAR;
    bool inverting_allowed = ! force_as_is_display;

    int i;
    STRLEN orig_sv_cur = SvCUR(sv);

    SV* invlist;            /* Inversion list we accumulate of code points that
                               are unconditionally matched */
    SV* only_utf8 = NULL;   /* Under /d, list of matches iff the target is
                               UTF-8 */
    SV* not_utf8 =  NULL;   /* /d, list of matches iff the target isn't UTF-8
                             */
    SV* posixes = NULL;     /* Under /l, string of things like [:word:], \D */
    SV* only_utf8_locale = NULL;    /* Under /l, list of matches if the locale
                                       is UTF-8 */

    SV* as_is_display;      /* The output string when we take the inputs
                               literally */
    SV* inverted_display;   /* The output string when we invert the inputs */

    U8 flags = (node) ? ANYOF_FLAGS(node) : 0;

    bool invert = cBOOL(flags & ANYOF_INVERT);  /* Is the input to be inverted
                                                   to match? */
    /* We are biased in favor of displaying things without them being inverted,
     * as that is generally easier to understand */
    const int bias = 5;

    PERL_ARGS_ASSERT_PUT_CHARCLASS_BITMAP_INNARDS;

    /* Start off with whatever code points are passed in.  (We clone, so we
     * don't change the caller's list) */
    if (nonbitmap_invlist) {
        assert(invlist_highest(nonbitmap_invlist) < NUM_ANYOF_CODE_POINTS);
        invlist = invlist_clone(nonbitmap_invlist, NULL);
    }
    else {  /* Worst case size is every other code point is matched */
        invlist = _new_invlist(NUM_ANYOF_CODE_POINTS / 2);
    }

    if (flags) {
        if (OP(node) == ANYOFD) {

            /* This flag indicates that the code points below 0x100 in the
             * nonbitmap list are precisely the ones that match only when the
             * target is UTF-8 (they should all be non-ASCII). */
            if (flags & ANYOF_SHARED_d_UPPER_LATIN1_UTF8_STRING_MATCHES_non_d_RUNTIME_USER_PROP)
            {
                _invlist_intersection(invlist, PL_UpperLatin1, &only_utf8);
                _invlist_subtract(invlist, only_utf8, &invlist);
            }

            /* And this flag for matching all non-ASCII 0xFF and below */
            if (flags & ANYOF_SHARED_d_MATCHES_ALL_NON_UTF8_NON_ASCII_non_d_WARN_SUPER)
            {
                not_utf8 = invlist_clone(PL_UpperLatin1, NULL);
            }
        }
        else if (OP(node) == ANYOFL || OP(node) == ANYOFPOSIXL) {

            /* If either of these flags are set, what matches isn't
             * determinable except during execution, so don't know enough here
             * to invert */
            if (flags & (ANYOFL_FOLD|ANYOF_MATCHES_POSIXL)) {
                inverting_allowed = FALSE;
            }

            /* What the posix classes match also varies at runtime, so these
             * will be output symbolically. */
            if (ANYOF_POSIXL_TEST_ANY_SET(node)) {
                int i;

                posixes = newSVpvs("");
                for (i = 0; i < ANYOF_POSIXL_MAX; i++) {
                    if (ANYOF_POSIXL_TEST(node, i)) {
                        sv_catpv(posixes, anyofs[i]);
                    }
                }
            }
        }
    }

    /* Accumulate the bit map into the unconditional match list */
    if (bitmap) {
        for (i = 0; i < NUM_ANYOF_CODE_POINTS; i++) {
            if (BITMAP_TEST(bitmap, i)) {
                int start = i++;
                for (;
                     i < NUM_ANYOF_CODE_POINTS && BITMAP_TEST(bitmap, i);
                     i++)
                { /* empty */ }
                invlist = _add_range_to_invlist(invlist, start, i-1);
            }
        }
    }

    /* Make sure that the conditional match lists don't have anything in them
     * that match unconditionally; otherwise the output is quite confusing.
     * This could happen if the code that populates these misses some
     * duplication. */
    if (only_utf8) {
        _invlist_subtract(only_utf8, invlist, &only_utf8);
    }
    if (not_utf8) {
        _invlist_subtract(not_utf8, invlist, &not_utf8);
    }

    if (only_utf8_locale_invlist) {

        /* Since this list is passed in, we have to make a copy before
         * modifying it */
        only_utf8_locale = invlist_clone(only_utf8_locale_invlist, NULL);

        _invlist_subtract(only_utf8_locale, invlist, &only_utf8_locale);

        /* And, it can get really weird for us to try outputting an inverted
         * form of this list when it has things above the bitmap, so don't even
         * try */
        if (invlist_highest(only_utf8_locale) >= NUM_ANYOF_CODE_POINTS) {
            inverting_allowed = FALSE;
        }
    }

    /* Calculate what the output would be if we take the input as-is */
    as_is_display = put_charclass_bitmap_innards_common(invlist,
                                                    posixes,
                                                    only_utf8,
                                                    not_utf8,
                                                    only_utf8_locale,
                                                    invert);

    /* If have to take the output as-is, just do that */
    if (! inverting_allowed) {
        if (as_is_display) {
            sv_catsv(sv, as_is_display);
            SvREFCNT_dec_NN(as_is_display);
        }
    }
    else { /* But otherwise, create the output again on the inverted input, and
              use whichever version is shorter */

        int inverted_bias, as_is_bias;

        /* We will apply our bias to whichever of the the results doesn't have
         * the '^' */
        if (invert) {
            invert = FALSE;
            as_is_bias = bias;
            inverted_bias = 0;
        }
        else {
            invert = TRUE;
            as_is_bias = 0;
            inverted_bias = bias;
        }

        /* Now invert each of the lists that contribute to the output,
         * excluding from the result things outside the possible range */

        /* For the unconditional inversion list, we have to add in all the
         * conditional code points, so that when inverted, they will be gone
         * from it */
        _invlist_union(only_utf8, invlist, &invlist);
        _invlist_union(not_utf8, invlist, &invlist);
        _invlist_union(only_utf8_locale, invlist, &invlist);
        _invlist_invert(invlist);
        _invlist_intersection(invlist, PL_InBitmap, &invlist);

        if (only_utf8) {
            _invlist_invert(only_utf8);
            _invlist_intersection(only_utf8, PL_UpperLatin1, &only_utf8);
        }
        else if (not_utf8) {

            /* If a code point matches iff the target string is not in UTF-8,
             * then complementing the result has it not match iff not in UTF-8,
             * which is the same thing as matching iff it is UTF-8. */
            only_utf8 = not_utf8;
            not_utf8 = NULL;
        }

        if (only_utf8_locale) {
            _invlist_invert(only_utf8_locale);
            _invlist_intersection(only_utf8_locale,
                                  PL_InBitmap,
                                  &only_utf8_locale);
        }

        inverted_display = put_charclass_bitmap_innards_common(
                                            invlist,
                                            posixes,
                                            only_utf8,
                                            not_utf8,
                                            only_utf8_locale, invert);

        /* Use the shortest representation, taking into account our bias
         * against showing it inverted */
        if (   inverted_display
            && (   ! as_is_display
                || (  SvCUR(inverted_display) + inverted_bias
                    < SvCUR(as_is_display)    + as_is_bias)))
        {
	    sv_catsv(sv, inverted_display);
        }
        else if (as_is_display) {
	    sv_catsv(sv, as_is_display);
        }

        SvREFCNT_dec(as_is_display);
        SvREFCNT_dec(inverted_display);
    }

    SvREFCNT_dec_NN(invlist);
    SvREFCNT_dec(only_utf8);
    SvREFCNT_dec(not_utf8);
    SvREFCNT_dec(posixes);
    SvREFCNT_dec(only_utf8_locale);

    return SvCUR(sv) > orig_sv_cur;