STATIC bool
S_grok_bslash_N(pTHX_ RExC_state_t *pRExC_state,
                regnode_offset * node_p,
                UV * code_point_p,
                int * cp_count,
                I32 * flagp,
                const bool strict,
                const U32 depth
    )
{
 /* This routine teases apart the various meanings of \N and returns
  * accordingly.  The input parameters constrain which meaning(s) is/are valid
  * in the current context.
  *
  * Exactly one of <node_p> and <code_point_p> must be non-NULL.
  *
  * If <code_point_p> is not NULL, the context is expecting the result to be a
  * single code point.  If this \N instance turns out to a single code point,
  * the function returns TRUE and sets *code_point_p to that code point.
  *
  * If <node_p> is not NULL, the context is expecting the result to be one of
  * the things representable by a regnode.  If this \N instance turns out to be
  * one such, the function generates the regnode, returns TRUE and sets *node_p
  * to point to the offset of that regnode into the regex engine program being
  * compiled.
  *
  * If this instance of \N isn't legal in any context, this function will
  * generate a fatal error and not return.
  *
  * On input, RExC_parse should point to the first char following the \N at the
  * time of the call.  On successful return, RExC_parse will have been updated
  * to point to just after the sequence identified by this routine.  Also
  * *flagp has been updated as needed.
  *
  * When there is some problem with the current context and this \N instance,
  * the function returns FALSE, without advancing RExC_parse, nor setting
  * *node_p, nor *code_point_p, nor *flagp.
  *
  * If <cp_count> is not NULL, the caller wants to know the length (in code
  * points) that this \N sequence matches.  This is set, and the input is
  * parsed for errors, even if the function returns FALSE, as detailed below.
  *
  * There are 6 possibilities here, as detailed in the next 6 paragraphs.
  *
  * Probably the most common case is for the \N to specify a single code point.
  * *cp_count will be set to 1, and *code_point_p will be set to that code
  * point.
  *
  * Another possibility is for the input to be an empty \N{}.  This is no
  * longer accepted, and will generate a fatal error.
  *
  * Another possibility is for a custom charnames handler to be in effect which
  * translates the input name to an empty string.  *cp_count will be set to 0.
  * *node_p will be set to a generated NOTHING node.
  *
  * Still another possibility is for the \N to mean [^\n]. *cp_count will be
  * set to 0. *node_p will be set to a generated REG_ANY node.
  *
  * The fifth possibility is that \N resolves to a sequence of more than one
  * code points.  *cp_count will be set to the number of code points in the
  * sequence. *node_p will be set to a generated node returned by this
  * function calling S_reg().
  *
  * The final possibility is that it is premature to be calling this function;
  * the parse needs to be restarted.  This can happen when this changes from
  * /d to /u rules, or when the pattern needs to be upgraded to UTF-8.  The
  * latter occurs only when the fifth possibility would otherwise be in
  * effect, and is because one of those code points requires the pattern to be
  * recompiled as UTF-8.  The function returns FALSE, and sets the
  * RESTART_PARSE and NEED_UTF8 flags in *flagp, as appropriate.  When this
  * happens, the caller needs to desist from continuing parsing, and return
  * this information to its caller.  This is not set for when there is only one
  * code point, as this can be called as part of an ANYOF node, and they can
  * store above-Latin1 code points without the pattern having to be in UTF-8.
  *
  * For non-single-quoted regexes, the tokenizer has resolved character and
  * sequence names inside \N{...} into their Unicode values, normalizing the
  * result into what we should see here: '\N{U+c1.c2...}', where c1... are the
  * hex-represented code points in the sequence.  This is done there because
  * the names can vary based on what charnames pragma is in scope at the time,
  * so we need a way to take a snapshot of what they resolve to at the time of
  * the original parse. [perl #56444].
  *
  * That parsing is skipped for single-quoted regexes, so here we may get
  * '\N{NAME}', which is parsed now.  If the single-quoted regex is something
  * like '\N{U+41}', that code point is Unicode, and has to be translated into
  * the native character set for non-ASCII platforms.  The other possibilities
  * are already native, so no translation is done. */

    char * endbrace;    /* points to '}' following the name */
    char* p = RExC_parse; /* Temporary */

    SV * substitute_parse = NULL;
    char *orig_end;
    char *save_start;
    I32 flags;

    GET_RE_DEBUG_FLAGS_DECL;

    PERL_ARGS_ASSERT_GROK_BSLASH_N;

    GET_RE_DEBUG_FLAGS;

    assert(cBOOL(node_p) ^ cBOOL(code_point_p));  /* Exactly one should be set */
    assert(! (node_p && cp_count));               /* At most 1 should be set */

    if (cp_count) {     /* Initialize return for the most common case */
        *cp_count = 1;
    }

    /* The [^\n] meaning of \N ignores spaces and comments under the /x
     * modifier.  The other meanings do not, so use a temporary until we find
     * out which we are being called with */
    skip_to_be_ignored_text(pRExC_state, &p,
                            FALSE /* Don't force to /x */ );

    /* Disambiguate between \N meaning a named character versus \N meaning
     * [^\n].  The latter is assumed when the {...} following the \N is a legal
     * quantifier, or if there is no '{' at all */
    if (*p != '{' || regcurly(p)) {
        RExC_parse = p;
        if (cp_count) {
            *cp_count = -1;
        }

        if (! node_p) {
            return FALSE;
        }

        *node_p = reg_node(pRExC_state, REG_ANY);
        *flagp |= HASWIDTH|SIMPLE;
        MARK_NAUGHTY(1);
        Set_Node_Length(REGNODE_p(*(node_p)), 1); /* MJD */
        return TRUE;
    }

    /* The test above made sure that the next real character is a '{', but
     * under the /x modifier, it could be separated by space (or a comment and
     * \n) and this is not allowed (for consistency with \x{...} and the
     * tokenizer handling of \N{NAME}). */
    if (*RExC_parse != '{') {
        vFAIL("Missing braces on \\N{}");
    }

    RExC_parse++;       /* Skip past the '{' */

    endbrace = (char *) memchr(RExC_parse, '}', RExC_end - RExC_parse);
    if (! endbrace) { /* no trailing brace */
        vFAIL2("Missing right brace on \\%c{}", 'N');
    }

    /* Here, we have decided it should be a named character or sequence.  These
     * imply Unicode semantics */
    REQUIRE_UNI_RULES(flagp, FALSE);

    /* \N{_} is what toke.c returns to us to indicate a name that evaluates to
     * nothing at all (not allowed under strict) */
    if (endbrace - RExC_parse == 1 && *RExC_parse == '_') {
        RExC_parse = endbrace;
        if (strict) {
            RExC_parse++;   /* Position after the "}" */
            vFAIL("Zero length \\N{}");
        }

        if (cp_count) {
            *cp_count = 0;
        }
        nextchar(pRExC_state);
        if (! node_p) {
            return FALSE;
        }

        *node_p = reg_node(pRExC_state, NOTHING);
        return TRUE;
    }

    if (endbrace - RExC_parse < 2 || ! strBEGINs(RExC_parse, "U+")) {

        /* Here, the name isn't of the form  U+....  This can happen if the
         * pattern is single-quoted, so didn't get evaluated in toke.c.  Now
         * is the time to find out what the name means */

        const STRLEN name_len = endbrace - RExC_parse;
        SV *  value_sv;     /* What does this name evaluate to */
        SV ** value_svp;
        const U8 * value;   /* string of name's value */
        STRLEN value_len;   /* and its length */

        /*  RExC_unlexed_names is a hash of names that weren't evaluated by
         *  toke.c, and their values. Make sure is initialized */
        if (! RExC_unlexed_names) {
            RExC_unlexed_names = newHV();
        }

        /* If we have already seen this name in this pattern, use that.  This
         * allows us to only call the charnames handler once per name per
         * pattern.  A broken or malicious handler could return something
         * different each time, which could cause the results to vary depending
         * on if something gets added or subtracted from the pattern that
         * causes the number of passes to change, for example */
        if ((value_svp = hv_fetch(RExC_unlexed_names, RExC_parse,
                                                      name_len, 0)))
        {
            value_sv = *value_svp;
        }
        else { /* Otherwise we have to go out and get the name */
            const char * error_msg = NULL;
            value_sv = get_and_check_backslash_N_name(RExC_parse, endbrace,
                                                      UTF,
                                                      &error_msg);
            if (error_msg) {
                RExC_parse = endbrace;
                vFAIL(error_msg);
            }

            /* If no error message, should have gotten a valid return */
            assert (value_sv);

            /* Save the name's meaning for later use */
            if (! hv_store(RExC_unlexed_names, RExC_parse, name_len,
                           value_sv, 0))
            {
                Perl_croak(aTHX_ "panic: hv_store() unexpectedly failed");
            }
        }

        /* Here, we have the value the name evaluates to in 'value_sv' */
        value = (U8 *) SvPV(value_sv, value_len);

        /* See if the result is one code point vs 0 or multiple */
        if (value_len > 0 && value_len <= (UV) ((SvUTF8(value_sv))
                                               ? UTF8SKIP(value)
                                               : 1))
        {
            /* Here, exactly one code point.  If that isn't what is wanted,
             * fail */
            if (! code_point_p) {
                RExC_parse = p;
                return FALSE;
            }

            /* Convert from string to numeric code point */
            *code_point_p = (SvUTF8(value_sv))
                            ? valid_utf8_to_uvchr(value, NULL)
                            : *value;

            /* Have parsed this entire single code point \N{...}.  *cp_count
             * has already been set to 1, so don't do it again. */
            RExC_parse = endbrace;
            nextchar(pRExC_state);
            return TRUE;
        } /* End of is a single code point */

        /* Count the code points, if caller desires.  The API says to do this
         * even if we will later return FALSE */
        if (cp_count) {
            *cp_count = 0;

            *cp_count = (SvUTF8(value_sv))
                        ? utf8_length(value, value + value_len)
                        : value_len;
        }

        /* Fail if caller doesn't want to handle a multi-code-point sequence.
         * But don't back the pointer up if the caller wants to know how many
         * code points there are (they need to handle it themselves in this
         * case).  */
        if (! node_p) {
            if (! cp_count) {
                RExC_parse = p;
            }
            return FALSE;
        }

        /* Convert this to a sub-pattern of the form "(?: ... )", and then call
         * reg recursively to parse it.  That way, it retains its atomicness,
         * while not having to worry about any special handling that some code
         * points may have. */

        substitute_parse = newSVpvs("?:");
        sv_catsv(substitute_parse, value_sv);
        sv_catpv(substitute_parse, ")");

#ifdef EBCDIC
        /* The value should already be native, so no need to convert on EBCDIC
         * platforms.*/
        assert(! RExC_recode_x_to_native);
#endif

    }
    else {   /* \N{U+...} */
        Size_t count = 0;   /* code point count kept internally */

        /* We can get to here when the input is \N{U+...} or when toke.c has
         * converted a name to the \N{U+...} form.  This include changing a
         * name that evaluates to multiple code points to \N{U+c1.c2.c3 ...} */

        RExC_parse += 2;    /* Skip past the 'U+' */

        /* Code points are separated by dots.  The '}' terminates the whole
         * thing. */

        do {    /* Loop until the ending brace */
            UV cp = 0;
            char * start_digit;     /* The first of the current code point */
            if (! isXDIGIT(*RExC_parse)) {
                RExC_parse++;
                vFAIL("Invalid hexadecimal number in \\N{U+...}");
            }

            start_digit = RExC_parse;
            count++;

            /* Loop through the hex digits of the current code point */
            do {
                /* Adding this digit will shift the result 4 bits.  If that
                 * result would be above the legal max, it's overflow */
                if (cp > MAX_LEGAL_CP >> 4) {

                    /* Find the end of the code point */
                    do {
                        RExC_parse ++;
                    } while (isXDIGIT(*RExC_parse) || *RExC_parse == '_');

                    /* Be sure to synchronize this message with the similar one
                     * in utf8.c */
                    vFAIL4("Use of code point 0x%.*s is not allowed; the"
                        " permissible max is 0x%" UVxf,
                        (int) (RExC_parse - start_digit), start_digit,
                        MAX_LEGAL_CP);
                }

                /* Accumulate this (valid) digit into the running total */
                cp  = (cp << 4) + READ_XDIGIT(RExC_parse);

                /* READ_XDIGIT advanced the input pointer.  Ignore a single
                 * underscore separator */
                if (*RExC_parse == '_' && isXDIGIT(RExC_parse[1])) {
                    RExC_parse++;
                }
            } while (isXDIGIT(*RExC_parse));

            /* Here, have accumulated the next code point */
            if (RExC_parse >= endbrace) {   /* If done ... */
                if (count != 1) {
                    goto do_concat;
                }

                /* Here, is a single code point; fail if doesn't want that */
                if (! code_point_p) {
                    RExC_parse = p;
                    return FALSE;
                }

                /* A single code point is easy to handle; just return it */
                *code_point_p = UNI_TO_NATIVE(cp);
                RExC_parse = endbrace;
                nextchar(pRExC_state);
                return TRUE;
            }

            /* Here, the only legal thing would be a multiple character
             * sequence (of the form "\N{U+c1.c2. ... }".   So the next
             * character must be a dot (and the one after that can't be the
             * endbrace, or we'd have something like \N{U+100.} ) */
            if (*RExC_parse != '.' || RExC_parse + 1 >= endbrace) {
                RExC_parse += (RExC_orig_utf8)  /* point to after 1st invalid */
                                ? UTF8SKIP(RExC_parse)
                                : 1;
                if (RExC_parse >= endbrace) { /* Guard against malformed utf8 */
                    RExC_parse = endbrace;
                }
                vFAIL("Invalid hexadecimal number in \\N{U+...}");
            }

            /* Here, looks like its really a multiple character sequence.  Fail
             * if that's not what the caller wants.  But continue with counting
             * and error checking if they still want a count */
            if (! node_p && ! cp_count) {
                return FALSE;
            }

            /* What is done here is to convert this to a sub-pattern of the
             * form \x{char1}\x{char2}...  and then call reg recursively to
             * parse it (enclosing in "(?: ... )" ).  That way, it retains its
             * atomicness, while not having to worry about special handling
             * that some code points may have.  We don't create a subpattern,
             * but go through the motions of code point counting and error
             * checking, if the caller doesn't want a node returned. */

            if (node_p && count == 1) {
                substitute_parse = newSVpvs("?:");
            }

          do_concat:

            if (node_p) {
                /* Convert to notation the rest of the code understands */
                sv_catpvs(substitute_parse, "\\x{");
                sv_catpvn(substitute_parse, start_digit,
                                            RExC_parse - start_digit);
                sv_catpvs(substitute_parse, "}");
            }

            /* Move to after the dot (or ending brace the final time through.)
             * */
            RExC_parse++;
            count++;

        } while (RExC_parse < endbrace);

        if (! node_p) { /* Doesn't want the node */
            assert (cp_count);

            *cp_count = count;
            return FALSE;
        }

        sv_catpvs(substitute_parse, ")");

#ifdef EBCDIC
        /* The values are Unicode, and therefore have to be converted to native
         * on a non-Unicode (meaning non-ASCII) platform. */
        RExC_recode_x_to_native = 1;
#endif

    }

    /* Here, we have the string the name evaluates to, ready to be parsed,
     * stored in 'substitute_parse' as a series of valid "\x{...}\x{...}"
     * constructs.  This can be called from within a substitute parse already.
     * The error reporting mechanism doesn't work for 2 levels of this, but the
     * code above has validated this new construct, so there should be no
     * errors generated by the below.  And this isn' an exact copy, so the
     * mechanism to seamlessly deal with this won't work, so turn off warnings
     * during it */
    save_start = RExC_start;
    orig_end = RExC_end;

    RExC_parse = RExC_start = SvPVX(substitute_parse);
    RExC_end = RExC_parse + SvCUR(substitute_parse);
    TURN_OFF_WARNINGS_IN_SUBSTITUTE_PARSE;

    *node_p = reg(pRExC_state, 1, &flags, depth+1);

    /* Restore the saved values */
    RESTORE_WARNINGS;
    RExC_start = save_start;
    RExC_parse = endbrace;
    RExC_end = orig_end;
#ifdef EBCDIC
    RExC_recode_x_to_native = 0;
#endif

    SvREFCNT_dec_NN(substitute_parse);

    if (! *node_p) {
        RETURN_FAIL_ON_RESTART(flags, flagp);
        FAIL2("panic: reg returned failure to grok_bslash_N, flags=%#" UVxf,
            (UV) flags);
    }
    *flagp |= flags&(HASWIDTH|SPSTART|SIMPLE|POSTPONED);

    nextchar(pRExC_state);

    return TRUE;