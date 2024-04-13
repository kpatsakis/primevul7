SV *
Perl_handle_user_defined_property(pTHX_

    /* Parses the contents of a user-defined property definition; returning the
     * expanded definition if possible.  If so, the return is an inversion
     * list.
     *
     * If there are subroutines that are part of the expansion and which aren't
     * known at the time of the call to this function, this returns what
     * parse_uniprop_string() returned for the first one encountered.
     *
     * If an error was found, NULL is returned, and 'msg' gets a suitable
     * message appended to it.  (Appending allows the back trace of how we got
     * to the faulty definition to be displayed through nested calls of
     * user-defined subs.)
     *
     * The caller IS responsible for freeing any returned SV.
     *
     * The syntax of the contents is pretty much described in perlunicode.pod,
     * but we also allow comments on each line */

    const char * name,          /* Name of property */
    const STRLEN name_len,      /* The name's length in bytes */
    const bool is_utf8,         /* ? Is 'name' encoded in UTF-8 */
    const bool to_fold,         /* ? Is this under /i */
    const bool runtime,         /* ? Are we in compile- or run-time */
    const bool deferrable,      /* Is it ok for this property's full definition
                                   to be deferred until later? */
    SV* contents,               /* The property's definition */
    bool *user_defined_ptr,     /* This will be set TRUE as we wouldn't be
                                   getting called unless this is thought to be
                                   a user-defined property */
    SV * msg,                   /* Any error or warning msg(s) are appended to
                                   this */
    const STRLEN level)         /* Recursion level of this call */
{
    STRLEN len;
    const char * string         = SvPV_const(contents, len);
    const char * const e        = string + len;
    const bool is_contents_utf8 = cBOOL(SvUTF8(contents));
    const STRLEN msgs_length_on_entry = SvCUR(msg);

    const char * s0 = string;   /* Points to first byte in the current line
                                   being parsed in 'string' */
    const char overflow_msg[] = "Code point too large in \"";
    SV* running_definition = NULL;

    PERL_ARGS_ASSERT_HANDLE_USER_DEFINED_PROPERTY;

    *user_defined_ptr = TRUE;

    /* Look at each line */
    while (s0 < e) {
        const char * s;     /* Current byte */
        char op = '+';      /* Default operation is 'union' */
        IV   min = 0;       /* range begin code point */
        IV   max = -1;      /* and range end */
        SV* this_definition;

        /* Skip comment lines */
        if (*s0 == '#') {
            s0 = strchr(s0, '\n');
            if (s0 == NULL) {
                break;
            }
            s0++;
            continue;
        }

        /* For backcompat, allow an empty first line */
        if (*s0 == '\n') {
            s0++;
            continue;
        }

        /* First character in the line may optionally be the operation */
        if (   *s0 == '+'
            || *s0 == '!'
            || *s0 == '-'
            || *s0 == '&')
        {
            op = *s0++;
        }

        /* If the line is one or two hex digits separated by blank space, its
         * a range; otherwise it is either another user-defined property or an
         * error */

        s = s0;

        if (! isXDIGIT(*s)) {
            goto check_if_property;
        }

        do { /* Each new hex digit will add 4 bits. */
            if (min > ( (IV) MAX_LEGAL_CP >> 4)) {
                s = strchr(s, '\n');
                if (s == NULL) {
                    s = e;
                }
                if (SvCUR(msg) > 0) sv_catpvs(msg, "; ");
                sv_catpv(msg, overflow_msg);
                Perl_sv_catpvf(aTHX_ msg, "%" UTF8f,
                                     UTF8fARG(is_contents_utf8, s - s0, s0));
                sv_catpvs(msg, "\"");
                goto return_failure;
            }

            /* Accumulate this digit into the value */
            min = (min << 4) + READ_XDIGIT(s);
        } while (isXDIGIT(*s));

        while (isBLANK(*s)) { s++; }

        /* We allow comments at the end of the line */
        if (*s == '#') {
            s = strchr(s, '\n');
            if (s == NULL) {
                s = e;
            }
            s++;
        }
        else if (s < e && *s != '\n') {
            if (! isXDIGIT(*s)) {
                goto check_if_property;
            }

            /* Look for the high point of the range */
            max = 0;
            do {
                if (max > ( (IV) MAX_LEGAL_CP >> 4)) {
                    s = strchr(s, '\n');
                    if (s == NULL) {
                        s = e;
                    }
                    if (SvCUR(msg) > 0) sv_catpvs(msg, "; ");
                    sv_catpv(msg, overflow_msg);
                    Perl_sv_catpvf(aTHX_ msg, "%" UTF8f,
                                      UTF8fARG(is_contents_utf8, s - s0, s0));
                    sv_catpvs(msg, "\"");
                    goto return_failure;
                }

                max = (max << 4) + READ_XDIGIT(s);
            } while (isXDIGIT(*s));

            while (isBLANK(*s)) { s++; }

            if (*s == '#') {
                s = strchr(s, '\n');
                if (s == NULL) {
                    s = e;
                }
            }
            else if (s < e && *s != '\n') {
                goto check_if_property;
            }
        }

        if (max == -1) {    /* The line only had one entry */
            max = min;
        }
        else if (max < min) {
            if (SvCUR(msg) > 0) sv_catpvs(msg, "; ");
            sv_catpvs(msg, "Illegal range in \"");
            Perl_sv_catpvf(aTHX_ msg, "%" UTF8f,
                                UTF8fARG(is_contents_utf8, s - s0, s0));
            sv_catpvs(msg, "\"");
            goto return_failure;
        }

#if 0   /* See explanation at definition above of get_extended_utf8_msg() */

        if (   UNICODE_IS_PERL_EXTENDED(min)
            || UNICODE_IS_PERL_EXTENDED(max))
        {
            if (SvCUR(msg) > 0) sv_catpvs(msg, "; ");

            /* If both code points are non-portable, warn only on the lower
             * one. */
            sv_catpv(msg, get_extended_utf8_msg(
                                            (UNICODE_IS_PERL_EXTENDED(min))
                                            ? min : max));
            sv_catpvs(msg, " in \"");
            Perl_sv_catpvf(aTHX_ msg, "%" UTF8f,
                                 UTF8fARG(is_contents_utf8, s - s0, s0));
            sv_catpvs(msg, "\"");
        }

#endif

        /* Here, this line contains a legal range */
        this_definition = sv_2mortal(_new_invlist(2));
        this_definition = _add_range_to_invlist(this_definition, min, max);
        goto calculate;

      check_if_property:

        /* Here it isn't a legal range line.  See if it is a legal property
         * line.  First find the end of the meat of the line */
        s = strpbrk(s, "#\n");
        if (s == NULL) {
            s = e;
        }

        /* Ignore trailing blanks in keeping with the requirements of
         * parse_uniprop_string() */
        s--;
        while (s > s0 && isBLANK_A(*s)) {
            s--;
        }
        s++;

        this_definition = parse_uniprop_string(s0, s - s0,
                                               is_utf8, to_fold, runtime,
                                               deferrable,
                                               user_defined_ptr, msg,
                                               (name_len == 0)
                                                ? level /* Don't increase level
                                                           if input is empty */
                                                : level + 1
                                              );
        if (this_definition == NULL) {
            goto return_failure;    /* 'msg' should have had the reason
                                       appended to it by the above call */
        }

        if (! is_invlist(this_definition)) {    /* Unknown at this time */
            return newSVsv(this_definition);
        }

        if (*s != '\n') {
            s = strchr(s, '\n');
            if (s == NULL) {
                s = e;
            }
        }

      calculate:

        switch (op) {
            case '+':
                _invlist_union(running_definition, this_definition,
                                                        &running_definition);
                break;
            case '-':
                _invlist_subtract(running_definition, this_definition,
                                                        &running_definition);
                break;
            case '&':
                _invlist_intersection(running_definition, this_definition,
                                                        &running_definition);
                break;
            case '!':
                _invlist_union_complement_2nd(running_definition,
                                        this_definition, &running_definition);
                break;
            default:
                Perl_croak(aTHX_ "panic: %s: %d: Unexpected operation %d",
                                 __FILE__, __LINE__, op);
                break;
        }

        /* Position past the '\n' */
        s0 = s + 1;
    }   /* End of loop through the lines of 'contents' */

    /* Here, we processed all the lines in 'contents' without error.  If we
     * didn't add any warnings, simply return success */
    if (msgs_length_on_entry == SvCUR(msg)) {

        /* If the expansion was empty, the answer isn't nothing: its an empty
         * inversion list */
        if (running_definition == NULL) {
            running_definition = _new_invlist(1);
        }

        return running_definition;
    }

    /* Otherwise, add some explanatory text, but we will return success */
    goto return_msg;

  return_failure:
    running_definition = NULL;

  return_msg:

    if (name_len > 0) {
        sv_catpvs(msg, " in expansion of ");
        Perl_sv_catpvf(aTHX_ msg, "%" UTF8f, UTF8fARG(is_utf8, name_len, name));
    }

    return running_definition;