STATIC int
S_handle_possible_posix(pTHX_ RExC_state_t *pRExC_state,

    const char * const s,      /* Where the putative posix class begins.
                                  Normally, this is one past the '['.  This
                                  parameter exists so it can be somewhere
                                  besides RExC_parse. */
    char ** updated_parse_ptr, /* Where to set the updated parse pointer, or
                                  NULL */
    AV ** posix_warnings,      /* Where to place any generated warnings, or
                                  NULL */
    const bool check_only      /* Don't die if error */
)
{
    /* This parses what the caller thinks may be one of the three POSIX
     * constructs:
     *  1) a character class, like [:blank:]
     *  2) a collating symbol, like [. .]
     *  3) an equivalence class, like [= =]
     * In the latter two cases, it croaks if it finds a syntactically legal
     * one, as these are not handled by Perl.
     *
     * The main purpose is to look for a POSIX character class.  It returns:
     *  a) the class number
     *      if it is a completely syntactically and semantically legal class.
     *      'updated_parse_ptr', if not NULL, is set to point to just after the
     *      closing ']' of the class
     *  b) OOB_NAMEDCLASS
     *      if it appears that one of the three POSIX constructs was meant, but
     *      its specification was somehow defective.  'updated_parse_ptr', if
     *      not NULL, is set to point to the character just after the end
     *      character of the class.  See below for handling of warnings.
     *  c) NOT_MEANT_TO_BE_A_POSIX_CLASS
     *      if it  doesn't appear that a POSIX construct was intended.
     *      'updated_parse_ptr' is not changed.  No warnings nor errors are
     *      raised.
     *
     * In b) there may be errors or warnings generated.  If 'check_only' is
     * TRUE, then any errors are discarded.  Warnings are returned to the
     * caller via an AV* created into '*posix_warnings' if it is not NULL.  If
     * instead it is NULL, warnings are suppressed.
     *
     * The reason for this function, and its complexity is that a bracketed
     * character class can contain just about anything.  But it's easy to
     * mistype the very specific posix class syntax but yielding a valid
     * regular bracketed class, so it silently gets compiled into something
     * quite unintended.
     *
     * The solution adopted here maintains backward compatibility except that
     * it adds a warning if it looks like a posix class was intended but
     * improperly specified.  The warning is not raised unless what is input
     * very closely resembles one of the 14 legal posix classes.  To do this,
     * it uses fuzzy parsing.  It calculates how many single-character edits it
     * would take to transform what was input into a legal posix class.  Only
     * if that number is quite small does it think that the intention was a
     * posix class.  Obviously these are heuristics, and there will be cases
     * where it errs on one side or another, and they can be tweaked as
     * experience informs.
     *
     * The syntax for a legal posix class is:
     *
     * qr/(?xa: \[ : \^? [[:lower:]]{4,6} : \] )/
     *
     * What this routine considers syntactically to be an intended posix class
     * is this (the comments indicate some restrictions that the pattern
     * doesn't show):
     *
     *  qr/(?x: \[?                         # The left bracket, possibly
     *                                      # omitted
     *          \h*                         # possibly followed by blanks
     *          (?: \^ \h* )?               # possibly a misplaced caret
     *          [:;]?                       # The opening class character,
     *                                      # possibly omitted.  A typo
     *                                      # semi-colon can also be used.
     *          \h*
     *          \^?                         # possibly a correctly placed
     *                                      # caret, but not if there was also
     *                                      # a misplaced one
     *          \h*
     *          .{3,15}                     # The class name.  If there are
     *                                      # deviations from the legal syntax,
     *                                      # its edit distance must be close
     *                                      # to a real class name in order
     *                                      # for it to be considered to be
     *                                      # an intended posix class.
     *          \h*
     *          [[:punct:]]?                # The closing class character,
     *                                      # possibly omitted.  If not a colon
     *                                      # nor semi colon, the class name
     *                                      # must be even closer to a valid
     *                                      # one
     *          \h*
     *          \]?                         # The right bracket, possibly
     *                                      # omitted.
     *     )/
     *
     * In the above, \h must be ASCII-only.
     *
     * These are heuristics, and can be tweaked as field experience dictates.
     * There will be cases when someone didn't intend to specify a posix class
     * that this warns as being so.  The goal is to minimize these, while
     * maximizing the catching of things intended to be a posix class that
     * aren't parsed as such.
     */

    const char* p             = s;
    const char * const e      = RExC_end;
    unsigned complement       = 0;      /* If to complement the class */
    bool found_problem        = FALSE;  /* Assume OK until proven otherwise */
    bool has_opening_bracket  = FALSE;
    bool has_opening_colon    = FALSE;
    int class_number          = OOB_NAMEDCLASS; /* Out-of-bounds until find
                                                   valid class */
    const char * possible_end = NULL;   /* used for a 2nd parse pass */
    const char* name_start;             /* ptr to class name first char */

    /* If the number of single-character typos the input name is away from a
     * legal name is no more than this number, it is considered to have meant
     * the legal name */
    int max_distance          = 2;

    /* to store the name.  The size determines the maximum length before we
     * decide that no posix class was intended.  Should be at least
     * sizeof("alphanumeric") */
    UV input_text[15];
    STATIC_ASSERT_DECL(C_ARRAY_LENGTH(input_text) >= sizeof "alphanumeric");

    PERL_ARGS_ASSERT_HANDLE_POSSIBLE_POSIX;

    CLEAR_POSIX_WARNINGS();

    if (p >= e) {
        return NOT_MEANT_TO_BE_A_POSIX_CLASS;
    }

    if (*(p - 1) != '[') {
        ADD_POSIX_WARNING(p, "it doesn't start with a '['");
        found_problem = TRUE;
    }
    else {
        has_opening_bracket = TRUE;
    }

    /* They could be confused and think you can put spaces between the
     * components */
    if (isBLANK(*p)) {
        found_problem = TRUE;

        do {
            p++;
        } while (p < e && isBLANK(*p));

        ADD_POSIX_WARNING(p, NO_BLANKS_POSIX_WARNING);
    }

    /* For [. .] and [= =].  These are quite different internally from [: :],
     * so they are handled separately.  */
    if (POSIXCC_NOTYET(*p) && p < e - 3) /* 1 for the close, and 1 for the ']'
                                            and 1 for at least one char in it
                                          */
    {
        const char open_char  = *p;
        const char * temp_ptr = p + 1;

        /* These two constructs are not handled by perl, and if we find a
         * syntactically valid one, we croak.  khw, who wrote this code, finds
         * this explanation of them very unclear:
         * http://pubs.opengroup.org/onlinepubs/009696899/basedefs/xbd_chap09.html
         * And searching the rest of the internet wasn't very helpful either.
         * It looks like just about any byte can be in these constructs,
         * depending on the locale.  But unless the pattern is being compiled
         * under /l, which is very rare, Perl runs under the C or POSIX locale.
         * In that case, it looks like [= =] isn't allowed at all, and that
         * [. .] could be any single code point, but for longer strings the
         * constituent characters would have to be the ASCII alphabetics plus
         * the minus-hyphen.  Any sensible locale definition would limit itself
         * to these.  And any portable one definitely should.  Trying to parse
         * the general case is a nightmare (see [perl #127604]).  So, this code
         * looks only for interiors of these constructs that match:
         *      qr/.|[-\w]{2,}/
         * Using \w relaxes the apparent rules a little, without adding much
         * danger of mistaking something else for one of these constructs.
         *
         * [. .] in some implementations described on the internet is usable to
         * escape a character that otherwise is special in bracketed character
         * classes.  For example [.].] means a literal right bracket instead of
         * the ending of the class
         *
         * [= =] can legitimately contain a [. .] construct, but we don't
         * handle this case, as that [. .] construct will later get parsed
         * itself and croak then.  And [= =] is checked for even when not under
         * /l, as Perl has long done so.
         *
         * The code below relies on there being a trailing NUL, so it doesn't
         * have to keep checking if the parse ptr < e.
         */
        if (temp_ptr[1] == open_char) {
            temp_ptr++;
        }
        else while (    temp_ptr < e
                    && (isWORDCHAR(*temp_ptr) || *temp_ptr == '-'))
        {
            temp_ptr++;
        }

        if (*temp_ptr == open_char) {
            temp_ptr++;
            if (*temp_ptr == ']') {
                temp_ptr++;
                if (! found_problem && ! check_only) {
                    RExC_parse = (char *) temp_ptr;
                    vFAIL3("POSIX syntax [%c %c] is reserved for future "
                            "extensions", open_char, open_char);
                }

                /* Here, the syntax wasn't completely valid, or else the call
                 * is to check-only */
                if (updated_parse_ptr) {
                    *updated_parse_ptr = (char *) temp_ptr;
                }

                CLEAR_POSIX_WARNINGS_AND_RETURN(OOB_NAMEDCLASS);
            }
        }

        /* If we find something that started out to look like one of these
         * constructs, but isn't, we continue below so that it can be checked
         * for being a class name with a typo of '.' or '=' instead of a colon.
         * */
    }

    /* Here, we think there is a possibility that a [: :] class was meant, and
     * we have the first real character.  It could be they think the '^' comes
     * first */
    if (*p == '^') {
        found_problem = TRUE;
        ADD_POSIX_WARNING(p + 1, "the '^' must come after the colon");
        complement = 1;
        p++;

        if (isBLANK(*p)) {
            found_problem = TRUE;

            do {
                p++;
            } while (p < e && isBLANK(*p));

            ADD_POSIX_WARNING(p, NO_BLANKS_POSIX_WARNING);
        }
    }

    /* But the first character should be a colon, which they could have easily
     * mistyped on a qwerty keyboard as a semi-colon (and which may be hard to
     * distinguish from a colon, so treat that as a colon).  */
    if (*p == ':') {
        p++;
        has_opening_colon = TRUE;
    }
    else if (*p == ';') {
        found_problem = TRUE;
        p++;
        ADD_POSIX_WARNING(p, SEMI_COLON_POSIX_WARNING);
        has_opening_colon = TRUE;
    }
    else {
        found_problem = TRUE;
        ADD_POSIX_WARNING(p, "there must be a starting ':'");

        /* Consider an initial punctuation (not one of the recognized ones) to
         * be a left terminator */
        if (*p != '^' && *p != ']' && isPUNCT(*p)) {
            p++;
        }
    }

    /* They may think that you can put spaces between the components */
    if (isBLANK(*p)) {
        found_problem = TRUE;

        do {
            p++;
        } while (p < e && isBLANK(*p));

        ADD_POSIX_WARNING(p, NO_BLANKS_POSIX_WARNING);
    }

    if (*p == '^') {

        /* We consider something like [^:^alnum:]] to not have been intended to
         * be a posix class, but XXX maybe we should */
        if (complement) {
            CLEAR_POSIX_WARNINGS_AND_RETURN(NOT_MEANT_TO_BE_A_POSIX_CLASS);
        }

        complement = 1;
        p++;
    }

    /* Again, they may think that you can put spaces between the components */
    if (isBLANK(*p)) {
        found_problem = TRUE;

        do {
            p++;
        } while (p < e && isBLANK(*p));

        ADD_POSIX_WARNING(p, NO_BLANKS_POSIX_WARNING);
    }

    if (*p == ']') {

        /* XXX This ']' may be a typo, and something else was meant.  But
         * treating it as such creates enough complications, that that
         * possibility isn't currently considered here.  So we assume that the
         * ']' is what is intended, and if we've already found an initial '[',
         * this leaves this construct looking like [:] or [:^], which almost
         * certainly weren't intended to be posix classes */
        if (has_opening_bracket) {
            CLEAR_POSIX_WARNINGS_AND_RETURN(NOT_MEANT_TO_BE_A_POSIX_CLASS);
        }

        /* But this function can be called when we parse the colon for
         * something like qr/[alpha:]]/, so we back up to look for the
         * beginning */
        p--;

        if (*p == ';') {
            found_problem = TRUE;
            ADD_POSIX_WARNING(p, SEMI_COLON_POSIX_WARNING);
        }
        else if (*p != ':') {

            /* XXX We are currently very restrictive here, so this code doesn't
             * consider the possibility that, say, /[alpha.]]/ was intended to
             * be a posix class. */
            CLEAR_POSIX_WARNINGS_AND_RETURN(NOT_MEANT_TO_BE_A_POSIX_CLASS);
        }

        /* Here we have something like 'foo:]'.  There was no initial colon,
         * and we back up over 'foo.  XXX Unlike the going forward case, we
         * don't handle typos of non-word chars in the middle */
        has_opening_colon = FALSE;
        p--;

        while (p > RExC_start && isWORDCHAR(*p)) {
            p--;
        }
        p++;

        /* Here, we have positioned ourselves to where we think the first
         * character in the potential class is */
    }

    /* Now the interior really starts.  There are certain key characters that
     * can end the interior, or these could just be typos.  To catch both
     * cases, we may have to do two passes.  In the first pass, we keep on
     * going unless we come to a sequence that matches
     *      qr/ [[:punct:]] [[:blank:]]* \] /xa
     * This means it takes a sequence to end the pass, so two typos in a row if
     * that wasn't what was intended.  If the class is perfectly formed, just
     * this one pass is needed.  We also stop if there are too many characters
     * being accumulated, but this number is deliberately set higher than any
     * real class.  It is set high enough so that someone who thinks that
     * 'alphanumeric' is a correct name would get warned that it wasn't.
     * While doing the pass, we keep track of where the key characters were in
     * it.  If we don't find an end to the class, and one of the key characters
     * was found, we redo the pass, but stop when we get to that character.
     * Thus the key character was considered a typo in the first pass, but a
     * terminator in the second.  If two key characters are found, we stop at
     * the second one in the first pass.  Again this can miss two typos, but
     * catches a single one
     *
     * In the first pass, 'possible_end' starts as NULL, and then gets set to
     * point to the first key character.  For the second pass, it starts as -1.
     * */

    name_start = p;
  parse_name:
    {
        bool has_blank               = FALSE;
        bool has_upper               = FALSE;
        bool has_terminating_colon   = FALSE;
        bool has_terminating_bracket = FALSE;
        bool has_semi_colon          = FALSE;
        unsigned int name_len        = 0;
        int punct_count              = 0;

        while (p < e) {

            /* Squeeze out blanks when looking up the class name below */
            if (isBLANK(*p) ) {
                has_blank = TRUE;
                found_problem = TRUE;
                p++;
                continue;
            }

            /* The name will end with a punctuation */
            if (isPUNCT(*p)) {
                const char * peek = p + 1;

                /* Treat any non-']' punctuation followed by a ']' (possibly
                 * with intervening blanks) as trying to terminate the class.
                 * ']]' is very likely to mean a class was intended (but
                 * missing the colon), but the warning message that gets
                 * generated shows the error position better if we exit the
                 * loop at the bottom (eventually), so skip it here. */
                if (*p != ']') {
                    if (peek < e && isBLANK(*peek)) {
                        has_blank = TRUE;
                        found_problem = TRUE;
                        do {
                            peek++;
                        } while (peek < e && isBLANK(*peek));
                    }

                    if (peek < e && *peek == ']') {
                        has_terminating_bracket = TRUE;
                        if (*p == ':') {
                            has_terminating_colon = TRUE;
                        }
                        else if (*p == ';') {
                            has_semi_colon = TRUE;
                            has_terminating_colon = TRUE;
                        }
                        else {
                            found_problem = TRUE;
                        }
                        p = peek + 1;
                        goto try_posix;
                    }
                }

                /* Here we have punctuation we thought didn't end the class.
                 * Keep track of the position of the key characters that are
                 * more likely to have been class-enders */
                if (*p == ']' || *p == '[' || *p == ':' || *p == ';') {

                    /* Allow just one such possible class-ender not actually
                     * ending the class. */
                    if (possible_end) {
                        break;
                    }
                    possible_end = p;
                }

                /* If we have too many punctuation characters, no use in
                 * keeping going */
                if (++punct_count > max_distance) {
                    break;
                }

                /* Treat the punctuation as a typo. */
                input_text[name_len++] = *p;
                p++;
            }
            else if (isUPPER(*p)) { /* Use lowercase for lookup */
                input_text[name_len++] = toLOWER(*p);
                has_upper = TRUE;
                found_problem = TRUE;
                p++;
            } else if (! UTF || UTF8_IS_INVARIANT(*p)) {
                input_text[name_len++] = *p;
                p++;
            }
            else {
                input_text[name_len++] = utf8_to_uvchr_buf((U8 *) p, e, NULL);
                p+= UTF8SKIP(p);
            }

            /* The declaration of 'input_text' is how long we allow a potential
             * class name to be, before saying they didn't mean a class name at
             * all */
            if (name_len >= C_ARRAY_LENGTH(input_text)) {
                break;
            }
        }

        /* We get to here when the possible class name hasn't been properly
         * terminated before:
         *   1) we ran off the end of the pattern; or
         *   2) found two characters, each of which might have been intended to
         *      be the name's terminator
         *   3) found so many punctuation characters in the purported name,
         *      that the edit distance to a valid one is exceeded
         *   4) we decided it was more characters than anyone could have
         *      intended to be one. */

        found_problem = TRUE;

        /* In the final two cases, we know that looking up what we've
         * accumulated won't lead to a match, even a fuzzy one. */
        if (   name_len >= C_ARRAY_LENGTH(input_text)
            || punct_count > max_distance)
        {
            /* If there was an intermediate key character that could have been
             * an intended end, redo the parse, but stop there */
            if (possible_end && possible_end != (char *) -1) {
                possible_end = (char *) -1; /* Special signal value to say
                                               we've done a first pass */
                p = name_start;
                goto parse_name;
            }

            /* Otherwise, it can't have meant to have been a class */
            CLEAR_POSIX_WARNINGS_AND_RETURN(NOT_MEANT_TO_BE_A_POSIX_CLASS);
        }

        /* If we ran off the end, and the final character was a punctuation
         * one, back up one, to look at that final one just below.  Later, we
         * will restore the parse pointer if appropriate */
        if (name_len && p == e && isPUNCT(*(p-1))) {
            p--;
            name_len--;
        }

        if (p < e && isPUNCT(*p)) {
            if (*p == ']') {
                has_terminating_bracket = TRUE;

                /* If this is a 2nd ']', and the first one is just below this
                 * one, consider that to be the real terminator.  This gives a
                 * uniform and better positioning for the warning message  */
                if (   possible_end
                    && possible_end != (char *) -1
                    && *possible_end == ']'
                    && name_len && input_text[name_len - 1] == ']')
                {
                    name_len--;
                    p = possible_end;

                    /* And this is actually equivalent to having done the 2nd
                     * pass now, so set it to not try again */
                    possible_end = (char *) -1;
                }
            }
            else {
                if (*p == ':') {
                    has_terminating_colon = TRUE;
                }
                else if (*p == ';') {
                    has_semi_colon = TRUE;
                    has_terminating_colon = TRUE;
                }
                p++;
            }
        }

    try_posix:

        /* Here, we have a class name to look up.  We can short circuit the
         * stuff below for short names that can't possibly be meant to be a
         * class name.  (We can do this on the first pass, as any second pass
         * will yield an even shorter name) */
        if (name_len < 3) {
            CLEAR_POSIX_WARNINGS_AND_RETURN(NOT_MEANT_TO_BE_A_POSIX_CLASS);
        }

        /* Find which class it is.  Initially switch on the length of the name.
         * */
        switch (name_len) {
            case 4:
                if (memEQs(name_start, 4, "word")) {
                    /* this is not POSIX, this is the Perl \w */
                    class_number = ANYOF_WORDCHAR;
                }
                break;
            case 5:
                /* Names all of length 5: alnum alpha ascii blank cntrl digit
                 *                        graph lower print punct space upper
                 * Offset 4 gives the best switch position.  */
                switch (name_start[4]) {
                    case 'a':
                        if (memBEGINs(name_start, 5, "alph")) /* alpha */
                            class_number = ANYOF_ALPHA;
                        break;
                    case 'e':
                        if (memBEGINs(name_start, 5, "spac")) /* space */
                            class_number = ANYOF_SPACE;
                        break;
                    case 'h':
                        if (memBEGINs(name_start, 5, "grap")) /* graph */
                            class_number = ANYOF_GRAPH;
                        break;
                    case 'i':
                        if (memBEGINs(name_start, 5, "asci")) /* ascii */
                            class_number = ANYOF_ASCII;
                        break;
                    case 'k':
                        if (memBEGINs(name_start, 5, "blan")) /* blank */
                            class_number = ANYOF_BLANK;
                        break;
                    case 'l':
                        if (memBEGINs(name_start, 5, "cntr")) /* cntrl */
                            class_number = ANYOF_CNTRL;
                        break;
                    case 'm':
                        if (memBEGINs(name_start, 5, "alnu")) /* alnum */
                            class_number = ANYOF_ALPHANUMERIC;
                        break;
                    case 'r':
                        if (memBEGINs(name_start, 5, "lowe")) /* lower */
                            class_number = (FOLD) ? ANYOF_CASED : ANYOF_LOWER;
                        else if (memBEGINs(name_start, 5, "uppe")) /* upper */
                            class_number = (FOLD) ? ANYOF_CASED : ANYOF_UPPER;
                        break;
                    case 't':
                        if (memBEGINs(name_start, 5, "digi")) /* digit */
                            class_number = ANYOF_DIGIT;
                        else if (memBEGINs(name_start, 5, "prin")) /* print */
                            class_number = ANYOF_PRINT;
                        else if (memBEGINs(name_start, 5, "punc")) /* punct */
                            class_number = ANYOF_PUNCT;
                        break;
                }
                break;
            case 6:
                if (memEQs(name_start, 6, "xdigit"))
                    class_number = ANYOF_XDIGIT;
                break;
        }

        /* If the name exactly matches a posix class name the class number will
         * here be set to it, and the input almost certainly was meant to be a
         * posix class, so we can skip further checking.  If instead the syntax
         * is exactly correct, but the name isn't one of the legal ones, we
         * will return that as an error below.  But if neither of these apply,
         * it could be that no posix class was intended at all, or that one
         * was, but there was a typo.  We tease these apart by doing fuzzy
         * matching on the name */
        if (class_number == OOB_NAMEDCLASS && found_problem) {
            const UV posix_names[][6] = {
                                                { 'a', 'l', 'n', 'u', 'm' },
                                                { 'a', 'l', 'p', 'h', 'a' },
                                                { 'a', 's', 'c', 'i', 'i' },
                                                { 'b', 'l', 'a', 'n', 'k' },
                                                { 'c', 'n', 't', 'r', 'l' },
                                                { 'd', 'i', 'g', 'i', 't' },
                                                { 'g', 'r', 'a', 'p', 'h' },
                                                { 'l', 'o', 'w', 'e', 'r' },
                                                { 'p', 'r', 'i', 'n', 't' },
                                                { 'p', 'u', 'n', 'c', 't' },
                                                { 's', 'p', 'a', 'c', 'e' },
                                                { 'u', 'p', 'p', 'e', 'r' },
                                                { 'w', 'o', 'r', 'd' },
                                                { 'x', 'd', 'i', 'g', 'i', 't' }
                                            };
            /* The names of the above all have added NULs to make them the same
             * size, so we need to also have the real lengths */
            const UV posix_name_lengths[] = {
                                                sizeof("alnum") - 1,
                                                sizeof("alpha") - 1,
                                                sizeof("ascii") - 1,
                                                sizeof("blank") - 1,
                                                sizeof("cntrl") - 1,
                                                sizeof("digit") - 1,
                                                sizeof("graph") - 1,
                                                sizeof("lower") - 1,
                                                sizeof("print") - 1,
                                                sizeof("punct") - 1,
                                                sizeof("space") - 1,
                                                sizeof("upper") - 1,
                                                sizeof("word")  - 1,
                                                sizeof("xdigit")- 1
                                            };
            unsigned int i;
            int temp_max = max_distance;    /* Use a temporary, so if we
                                               reparse, we haven't changed the
                                               outer one */

            /* Use a smaller max edit distance if we are missing one of the
             * delimiters */
            if (   has_opening_bracket + has_opening_colon < 2
                || has_terminating_bracket + has_terminating_colon < 2)
            {
                temp_max--;
            }

            /* See if the input name is close to a legal one */
            for (i = 0; i < C_ARRAY_LENGTH(posix_names); i++) {

                /* Short circuit call if the lengths are too far apart to be
                 * able to match */
                if (abs( (int) (name_len - posix_name_lengths[i]))
                    > temp_max)
                {
                    continue;
                }

                if (edit_distance(input_text,
                                  posix_names[i],
                                  name_len,
                                  posix_name_lengths[i],
                                  temp_max
                                 )
                    > -1)
                { /* If it is close, it probably was intended to be a class */
                    goto probably_meant_to_be;
                }
            }

            /* Here the input name is not close enough to a valid class name
             * for us to consider it to be intended to be a posix class.  If
             * we haven't already done so, and the parse found a character that
             * could have been terminators for the name, but which we absorbed
             * as typos during the first pass, repeat the parse, signalling it
             * to stop at that character */
            if (possible_end && possible_end != (char *) -1) {
                possible_end = (char *) -1;
                p = name_start;
                goto parse_name;
            }

            /* Here neither pass found a close-enough class name */
            CLEAR_POSIX_WARNINGS_AND_RETURN(NOT_MEANT_TO_BE_A_POSIX_CLASS);
        }

    probably_meant_to_be:

        /* Here we think that a posix specification was intended.  Update any
         * parse pointer */
        if (updated_parse_ptr) {
            *updated_parse_ptr = (char *) p;
        }

        /* If a posix class name was intended but incorrectly specified, we
         * output or return the warnings */
        if (found_problem) {

            /* We set flags for these issues in the parse loop above instead of
             * adding them to the list of warnings, because we can parse it
             * twice, and we only want one warning instance */
            if (has_upper) {
                ADD_POSIX_WARNING(p, "the name must be all lowercase letters");
            }
            if (has_blank) {
                ADD_POSIX_WARNING(p, NO_BLANKS_POSIX_WARNING);
            }
            if (has_semi_colon) {
                ADD_POSIX_WARNING(p, SEMI_COLON_POSIX_WARNING);
            }
            else if (! has_terminating_colon) {
                ADD_POSIX_WARNING(p, "there is no terminating ':'");
            }
            if (! has_terminating_bracket) {
                ADD_POSIX_WARNING(p, "there is no terminating ']'");
            }

            if (   posix_warnings
                && RExC_warn_text
                && av_top_index(RExC_warn_text) > -1)
            {
                *posix_warnings = RExC_warn_text;
            }
        }
        else if (class_number != OOB_NAMEDCLASS) {
            /* If it is a known class, return the class.  The class number
             * #defines are structured so each complement is +1 to the normal
             * one */
            CLEAR_POSIX_WARNINGS_AND_RETURN(class_number + complement);
        }
        else if (! check_only) {

            /* Here, it is an unrecognized class.  This is an error (unless the
            * call is to check only, which we've already handled above) */
            const char * const complement_string = (complement)
                                                   ? "^"
                                                   : "";
            RExC_parse = (char *) p;
            vFAIL3utf8f("POSIX class [:%s%" UTF8f ":] unknown",
                        complement_string,
                        UTF8fARG(UTF, RExC_parse - name_start - 2, name_start));
        }
    }

    return OOB_NAMEDCLASS;