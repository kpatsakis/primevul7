SV *
Perl__get_regclass_nonbitmap_data(pTHX_ const regexp *prog,
                                        const regnode* node,
                                        bool doinit,
                                        SV** listsvp,
                                        SV** only_utf8_locale_ptr,
                                        SV** output_invlist)

{
    /* For internal core use only.
     * Returns the inversion list for the input 'node' in the regex 'prog'.
     * If <doinit> is 'true', will attempt to create the inversion list if not
     *    already done.
     * If <listsvp> is non-null, will return the printable contents of the
     *    property definition.  This can be used to get debugging information
     *    even before the inversion list exists, by calling this function with
     *    'doinit' set to false, in which case the components that will be used
     *    to eventually create the inversion list are returned  (in a printable
     *    form).
     * If <only_utf8_locale_ptr> is not NULL, it is where this routine is to
     *    store an inversion list of code points that should match only if the
     *    execution-time locale is a UTF-8 one.
     * If <output_invlist> is not NULL, it is where this routine is to store an
     *    inversion list of the code points that would be instead returned in
     *    <listsvp> if this were NULL.  Thus, what gets output in <listsvp>
     *    when this parameter is used, is just the non-code point data that
     *    will go into creating the inversion list.  This currently should be just
     *    user-defined properties whose definitions were not known at compile
     *    time.  Using this parameter allows for easier manipulation of the
     *    inversion list's data by the caller.  It is illegal to call this
     *    function with this parameter set, but not <listsvp>
     *
     * Tied intimately to how S_set_ANYOF_arg sets up the data structure.  Note
     * that, in spite of this function's name, the inversion list it returns
     * may include the bitmap data as well */

    SV *si  = NULL;         /* Input initialization string */
    SV* invlist = NULL;

    RXi_GET_DECL(prog, progi);
    const struct reg_data * const data = prog ? progi->data : NULL;

    PERL_ARGS_ASSERT__GET_REGCLASS_NONBITMAP_DATA;
    assert(! output_invlist || listsvp);

    if (data && data->count) {
	const U32 n = ARG(node);

	if (data->what[n] == 's') {
	    SV * const rv = MUTABLE_SV(data->data[n]);
	    AV * const av = MUTABLE_AV(SvRV(rv));
	    SV **const ary = AvARRAY(av);

            invlist = ary[INVLIST_INDEX];

            if (av_tindex_skip_len_mg(av) >= ONLY_LOCALE_MATCHES_INDEX) {
                *only_utf8_locale_ptr = ary[ONLY_LOCALE_MATCHES_INDEX];
            }

            if (av_tindex_skip_len_mg(av) >= DEFERRED_USER_DEFINED_INDEX) {
                si = ary[DEFERRED_USER_DEFINED_INDEX];
            }

	    if (doinit && (si || invlist)) {
                if (si) {
                    bool user_defined;
                    SV * msg = newSVpvs_flags("", SVs_TEMP);

                    SV * prop_definition = handle_user_defined_property(
                            "", 0, FALSE,   /* There is no \p{}, \P{} */
                            SvPVX_const(si)[1] - '0',   /* /i or not has been
                                                           stored here for just
                                                           this occasion */
                            TRUE,           /* run time */
                            FALSE,          /* This call must find the defn */
                            si,             /* The property definition  */
                            &user_defined,
                            msg,
                            0               /* base level call */
                           );

                    if (SvCUR(msg)) {
                        assert(prop_definition == NULL);

                        Perl_croak(aTHX_ "%" UTF8f,
                                UTF8fARG(SvUTF8(msg), SvCUR(msg), SvPVX(msg)));
                    }

                    if (invlist) {
                        _invlist_union(invlist, prop_definition, &invlist);
                        SvREFCNT_dec_NN(prop_definition);
                    }
                    else {
                        invlist = prop_definition;
                    }

                    STATIC_ASSERT_STMT(ONLY_LOCALE_MATCHES_INDEX == 1 + INVLIST_INDEX);
                    STATIC_ASSERT_STMT(DEFERRED_USER_DEFINED_INDEX == 1 + ONLY_LOCALE_MATCHES_INDEX);

                    av_store(av, INVLIST_INDEX, invlist);
                    av_fill(av, (ary[ONLY_LOCALE_MATCHES_INDEX])
                                 ? ONLY_LOCALE_MATCHES_INDEX:
                                 INVLIST_INDEX);
                    si = NULL;
                }
	    }
	}
    }

    /* If requested, return a printable version of what this ANYOF node matches
     * */
    if (listsvp) {
	SV* matches_string = NULL;

        /* This function can be called at compile-time, before everything gets
         * resolved, in which case we return the currently best available
         * information, which is the string that will eventually be used to do
         * that resolving, 'si' */
	if (si) {
            /* Here, we only have 'si' (and possibly some passed-in data in
             * 'invlist', which is handled below)  If the caller only wants
             * 'si', use that.  */
            if (! output_invlist) {
                matches_string = newSVsv(si);
            }
            else {
                /* But if the caller wants an inversion list of the node, we
                 * need to parse 'si' and place as much as possible in the
                 * desired output inversion list, making 'matches_string' only
                 * contain the currently unresolvable things */
                const char *si_string = SvPVX(si);
                STRLEN remaining = SvCUR(si);
                UV prev_cp = 0;
                U8 count = 0;

                /* Ignore everything before the first new-line */
                while (*si_string != '\n' && remaining > 0) {
                    si_string++;
                    remaining--;
                }
                assert(remaining > 0);

                si_string++;
                remaining--;

                while (remaining > 0) {

                    /* The data consists of just strings defining user-defined
                     * property names, but in prior incarnations, and perhaps
                     * somehow from pluggable regex engines, it could still
                     * hold hex code point definitions.  Each component of a
                     * range would be separated by a tab, and each range by a
                     * new-line.  If these are found, instead add them to the
                     * inversion list */
                    I32 grok_flags =  PERL_SCAN_SILENT_ILLDIGIT
                                     |PERL_SCAN_SILENT_NON_PORTABLE;
                    STRLEN len = remaining;
                    UV cp = grok_hex(si_string, &len, &grok_flags, NULL);

                    /* If the hex decode routine found something, it should go
                     * up to the next \n */
                    if (   *(si_string + len) == '\n') {
                        if (count) {    /* 2nd code point on line */
                            *output_invlist = _add_range_to_invlist(*output_invlist, prev_cp, cp);
                        }
                        else {
                            *output_invlist = add_cp_to_invlist(*output_invlist, cp);
                        }
                        count = 0;
                        goto prepare_for_next_iteration;
                    }

                    /* If the hex decode was instead for the lower range limit,
                     * save it, and go parse the upper range limit */
                    if (*(si_string + len) == '\t') {
                        assert(count == 0);

                        prev_cp = cp;
                        count = 1;
                      prepare_for_next_iteration:
                        si_string += len + 1;
                        remaining -= len + 1;
                        continue;
                    }

                    /* Here, didn't find a legal hex number.  Just add it from
                     * here to the next \n */

                    remaining -= len;
                    while (*(si_string + len) != '\n' && remaining > 0) {
                        remaining--;
                        len++;
                    }
                    if (*(si_string + len) == '\n') {
                        len++;
                        remaining--;
                    }
                    if (matches_string) {
                        sv_catpvn(matches_string, si_string, len - 1);
                    }
                    else {
                        matches_string = newSVpvn(si_string, len - 1);
                    }
                    si_string += len;
                    sv_catpvs(matches_string, " ");
                } /* end of loop through the text */

                assert(matches_string);
                if (SvCUR(matches_string)) {  /* Get rid of trailing blank */
                    SvCUR_set(matches_string, SvCUR(matches_string) - 1);
                }
            } /* end of has an 'si' */
	}

        /* Add the stuff that's already known */
        if (invlist) {

            /* Again, if the caller doesn't want the output inversion list, put
             * everything in 'matches-string' */
            if (! output_invlist) {
                if ( ! matches_string) {
                    matches_string = newSVpvs("\n");
                }
                sv_catsv(matches_string, invlist_contents(invlist,
                                                  TRUE /* traditional style */
                                                  ));
            }
            else if (! *output_invlist) {
                *output_invlist = invlist_clone(invlist, NULL);
            }
            else {
                _invlist_union(*output_invlist, invlist, output_invlist);
            }
        }

	*listsvp = matches_string;
    }

    return invlist;