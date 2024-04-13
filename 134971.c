STATIC regnode_offset
S_regclass(pTHX_ RExC_state_t *pRExC_state, I32 *flagp, U32 depth,
                 const bool stop_at_1,  /* Just parse the next thing, don't
                                           look for a full character class */
                 bool allow_mutiple_chars,
                 const bool silence_non_portable,   /* Don't output warnings
                                                       about too large
                                                       characters */
                 const bool strict,
                 bool optimizable,                  /* ? Allow a non-ANYOF return
                                                       node */
                 SV** ret_invlist  /* Return an inversion list, not a node */
          )
{
    /* parse a bracketed class specification.  Most of these will produce an
     * ANYOF node; but something like [a] will produce an EXACT node; [aA], an
     * EXACTFish node; [[:ascii:]], a POSIXA node; etc.  It is more complex
     * under /i with multi-character folds: it will be rewritten following the
     * paradigm of this example, where the <multi-fold>s are characters which
     * fold to multiple character sequences:
     *      /[abc\x{multi-fold1}def\x{multi-fold2}ghi]/i
     * gets effectively rewritten as:
     *      /(?:\x{multi-fold1}|\x{multi-fold2}|[abcdefghi]/i
     * reg() gets called (recursively) on the rewritten version, and this
     * function will return what it constructs.  (Actually the <multi-fold>s
     * aren't physically removed from the [abcdefghi], it's just that they are
     * ignored in the recursion by means of a flag:
     * <RExC_in_multi_char_class>.)
     *
     * ANYOF nodes contain a bit map for the first NUM_ANYOF_CODE_POINTS
     * characters, with the corresponding bit set if that character is in the
     * list.  For characters above this, an inversion list is used.  There
     * are extra bits for \w, etc. in locale ANYOFs, as what these match is not
     * determinable at compile time
     *
     * On success, returns the offset at which any next node should be placed
     * into the regex engine program being compiled.
     *
     * Returns 0 otherwise, setting flagp to RESTART_PARSE if the parse needs
     * to be restarted, or'd with NEED_UTF8 if the pattern needs to be upgraded to
     * UTF-8
     */

    dVAR;
    UV prevvalue = OOB_UNICODE, save_prevvalue = OOB_UNICODE;
    IV range = 0;
    UV value = OOB_UNICODE, save_value = OOB_UNICODE;
    regnode_offset ret = -1;    /* Initialized to an illegal value */
    STRLEN numlen;
    int namedclass = OOB_NAMEDCLASS;
    char *rangebegin = NULL;
    SV *listsv = NULL;      /* List of \p{user-defined} whose definitions
                               aren't available at the time this was called */
    STRLEN initial_listsv_len = 0; /* Kind of a kludge to see if it is more
				      than just initialized.  */
    SV* properties = NULL;    /* Code points that match \p{} \P{} */
    SV* posixes = NULL;     /* Code points that match classes like [:word:],
                               extended beyond the Latin1 range.  These have to
                               be kept separate from other code points for much
                               of this function because their handling  is
                               different under /i, and for most classes under
                               /d as well */
    SV* nposixes = NULL;    /* Similarly for [:^word:].  These are kept
                               separate for a while from the non-complemented
                               versions because of complications with /d
                               matching */
    SV* simple_posixes = NULL; /* But under some conditions, the classes can be
                                  treated more simply than the general case,
                                  leading to less compilation and execution
                                  work */
    UV element_count = 0;   /* Number of distinct elements in the class.
			       Optimizations may be possible if this is tiny */
    AV * multi_char_matches = NULL; /* Code points that fold to more than one
                                       character; used under /i */
    UV n;
    char * stop_ptr = RExC_end;    /* where to stop parsing */

    /* ignore unescaped whitespace? */
    const bool skip_white = cBOOL(   ret_invlist
                                  || (RExC_flags & RXf_PMf_EXTENDED_MORE));

    /* inversion list of code points this node matches only when the target
     * string is in UTF-8.  These are all non-ASCII, < 256.  (Because is under
     * /d) */
    SV* upper_latin1_only_utf8_matches = NULL;

    /* Inversion list of code points this node matches regardless of things
     * like locale, folding, utf8ness of the target string */
    SV* cp_list = NULL;

    /* Like cp_list, but code points on this list need to be checked for things
     * that fold to/from them under /i */
    SV* cp_foldable_list = NULL;

    /* Like cp_list, but code points on this list are valid only when the
     * runtime locale is UTF-8 */
    SV* only_utf8_locale_list = NULL;

    /* In a range, if one of the endpoints is non-character-set portable,
     * meaning that it hard-codes a code point that may mean a different
     * charactger in ASCII vs. EBCDIC, as opposed to, say, a literal 'A' or a
     * mnemonic '\t' which each mean the same character no matter which
     * character set the platform is on. */
    unsigned int non_portable_endpoint = 0;

    /* Is the range unicode? which means on a platform that isn't 1-1 native
     * to Unicode (i.e. non-ASCII), each code point in it should be considered
     * to be a Unicode value.  */
    bool unicode_range = FALSE;
    bool invert = FALSE;    /* Is this class to be complemented */

    bool warn_super = ALWAYS_WARN_SUPER;

    const char * orig_parse = RExC_parse;

    /* This variable is used to mark where the end in the input is of something
     * that looks like a POSIX construct but isn't.  During the parse, when
     * something looks like it could be such a construct is encountered, it is
     * checked for being one, but not if we've already checked this area of the
     * input.  Only after this position is reached do we check again */
    char *not_posix_region_end = RExC_parse - 1;

    AV* posix_warnings = NULL;
    const bool do_posix_warnings = ckWARN(WARN_REGEXP);
    U8 op = END;    /* The returned node-type, initialized to an impossible
                       one.  */
    U8 anyof_flags = 0;   /* flag bits if the node is an ANYOF-type */
    U32 posixl = 0;       /* bit field of posix classes matched under /l */


/* Flags as to what things aren't knowable until runtime.  (Note that these are
 * mutually exclusive.) */
#define HAS_USER_DEFINED_PROPERTY 0x01   /* /u any user-defined properties that
                                            haven't been defined as of yet */
#define HAS_D_RUNTIME_DEPENDENCY  0x02   /* /d if the target being matched is
                                            UTF-8 or not */
#define HAS_L_RUNTIME_DEPENDENCY   0x04 /* /l what the posix classes match and
                                            what gets folded */
    U32 has_runtime_dependency = 0;     /* OR of the above flags */

    GET_RE_DEBUG_FLAGS_DECL;

    PERL_ARGS_ASSERT_REGCLASS;
#ifndef DEBUGGING
    PERL_UNUSED_ARG(depth);
#endif


    /* If wants an inversion list returned, we can't optimize to something
     * else. */
    if (ret_invlist) {
        optimizable = FALSE;
    }

    DEBUG_PARSE("clas");

#if UNICODE_MAJOR_VERSION < 3 /* no multifolds in early Unicode */      \
    || (UNICODE_MAJOR_VERSION == 3 && UNICODE_DOT_VERSION == 0          \
                                   && UNICODE_DOT_DOT_VERSION == 0)
    allow_mutiple_chars = FALSE;
#endif

    /* We include the /i status at the beginning of this so that we can
     * know it at runtime */
    listsv = sv_2mortal(Perl_newSVpvf(aTHX_ "#%d\n", cBOOL(FOLD)));
    initial_listsv_len = SvCUR(listsv);
    SvTEMP_off(listsv); /* Grr, TEMPs and mortals are conflated.  */

    SKIP_BRACKETED_WHITE_SPACE(skip_white, RExC_parse);

    assert(RExC_parse <= RExC_end);

    if (UCHARAT(RExC_parse) == '^') {	/* Complement the class */
	RExC_parse++;
        invert = TRUE;
        allow_mutiple_chars = FALSE;
        MARK_NAUGHTY(1);
        SKIP_BRACKETED_WHITE_SPACE(skip_white, RExC_parse);
    }

    /* Check that they didn't say [:posix:] instead of [[:posix:]] */
    if (! ret_invlist && MAYBE_POSIXCC(UCHARAT(RExC_parse))) {
        int maybe_class = handle_possible_posix(pRExC_state,
                                                RExC_parse,
                                                &not_posix_region_end,
                                                NULL,
                                                TRUE /* checking only */);
        if (maybe_class >= OOB_NAMEDCLASS && do_posix_warnings) {
            ckWARN4reg(not_posix_region_end,
                    "POSIX syntax [%c %c] belongs inside character classes%s",
                    *RExC_parse, *RExC_parse,
                    (maybe_class == OOB_NAMEDCLASS)
                    ? ((POSIXCC_NOTYET(*RExC_parse))
                        ? " (but this one isn't implemented)"
                        : " (but this one isn't fully valid)")
                    : ""
                    );
        }
    }

    /* If the caller wants us to just parse a single element, accomplish this
     * by faking the loop ending condition */
    if (stop_at_1 && RExC_end > RExC_parse) {
        stop_ptr = RExC_parse + 1;
    }

    /* allow 1st char to be ']' (allowing it to be '-' is dealt with later) */
    if (UCHARAT(RExC_parse) == ']')
	goto charclassloop;

    while (1) {

        if (   posix_warnings
            && av_tindex_skip_len_mg(posix_warnings) >= 0
            && RExC_parse > not_posix_region_end)
        {
            /* Warnings about posix class issues are considered tentative until
             * we are far enough along in the parse that we can no longer
             * change our mind, at which point we output them.  This is done
             * each time through the loop so that a later class won't zap them
             * before they have been dealt with. */
            output_posix_warnings(pRExC_state, posix_warnings);
        }

        if  (RExC_parse >= stop_ptr) {
            break;
        }

        SKIP_BRACKETED_WHITE_SPACE(skip_white, RExC_parse);

        if  (UCHARAT(RExC_parse) == ']') {
            break;
        }

      charclassloop:

	namedclass = OOB_NAMEDCLASS; /* initialize as illegal */
        save_value = value;
        save_prevvalue = prevvalue;

	if (!range) {
	    rangebegin = RExC_parse;
	    element_count++;
            non_portable_endpoint = 0;
	}
	if (UTF && ! UTF8_IS_INVARIANT(* RExC_parse)) {
	    value = utf8n_to_uvchr((U8*)RExC_parse,
				   RExC_end - RExC_parse,
				   &numlen, UTF8_ALLOW_DEFAULT);
	    RExC_parse += numlen;
	}
	else
	    value = UCHARAT(RExC_parse++);

        if (value == '[') {
            char * posix_class_end;
            namedclass = handle_possible_posix(pRExC_state,
                                               RExC_parse,
                                               &posix_class_end,
                                               do_posix_warnings ? &posix_warnings : NULL,
                                               FALSE    /* die if error */);
            if (namedclass > OOB_NAMEDCLASS) {

                /* If there was an earlier attempt to parse this particular
                 * posix class, and it failed, it was a false alarm, as this
                 * successful one proves */
                if (   posix_warnings
                    && av_tindex_skip_len_mg(posix_warnings) >= 0
                    && not_posix_region_end >= RExC_parse
                    && not_posix_region_end <= posix_class_end)
                {
                    av_undef(posix_warnings);
                }

                RExC_parse = posix_class_end;
            }
            else if (namedclass == OOB_NAMEDCLASS) {
                not_posix_region_end = posix_class_end;
            }
            else {
                namedclass = OOB_NAMEDCLASS;
            }
        }
        else if (   RExC_parse - 1 > not_posix_region_end
                 && MAYBE_POSIXCC(value))
        {
            (void) handle_possible_posix(
                        pRExC_state,
                        RExC_parse - 1,  /* -1 because parse has already been
                                            advanced */
                        &not_posix_region_end,
                        do_posix_warnings ? &posix_warnings : NULL,
                        TRUE /* checking only */);
        }
        else if (  strict && ! skip_white
                 && (   _generic_isCC(value, _CC_VERTSPACE)
                     || is_VERTWS_cp_high(value)))
        {
            vFAIL("Literal vertical space in [] is illegal except under /x");
        }
        else if (value == '\\') {
            /* Is a backslash; get the code point of the char after it */

            if (RExC_parse >= RExC_end) {
                vFAIL("Unmatched [");
            }

	    if (UTF && ! UTF8_IS_INVARIANT(UCHARAT(RExC_parse))) {
		value = utf8n_to_uvchr((U8*)RExC_parse,
				   RExC_end - RExC_parse,
				   &numlen, UTF8_ALLOW_DEFAULT);
		RExC_parse += numlen;
	    }
	    else
		value = UCHARAT(RExC_parse++);

	    /* Some compilers cannot handle switching on 64-bit integer
	     * values, therefore value cannot be an UV.  Yes, this will
	     * be a problem later if we want switch on Unicode.
	     * A similar issue a little bit later when switching on
	     * namedclass. --jhi */

            /* If the \ is escaping white space when white space is being
             * skipped, it means that that white space is wanted literally, and
             * is already in 'value'.  Otherwise, need to translate the escape
             * into what it signifies. */
            if (! skip_white || ! isBLANK_A(value)) switch ((I32)value) {

	    case 'w':	namedclass = ANYOF_WORDCHAR;	break;
	    case 'W':	namedclass = ANYOF_NWORDCHAR;	break;
	    case 's':	namedclass = ANYOF_SPACE;	break;
	    case 'S':	namedclass = ANYOF_NSPACE;	break;
	    case 'd':	namedclass = ANYOF_DIGIT;	break;
	    case 'D':	namedclass = ANYOF_NDIGIT;	break;
	    case 'v':	namedclass = ANYOF_VERTWS;	break;
	    case 'V':	namedclass = ANYOF_NVERTWS;	break;
	    case 'h':	namedclass = ANYOF_HORIZWS;	break;
	    case 'H':	namedclass = ANYOF_NHORIZWS;	break;
            case 'N':  /* Handle \N{NAME} in class */
                {
                    const char * const backslash_N_beg = RExC_parse - 2;
                    int cp_count;

                    if (! grok_bslash_N(pRExC_state,
                                        NULL,      /* No regnode */
                                        &value,    /* Yes single value */
                                        &cp_count, /* Multiple code pt count */
                                        flagp,
                                        strict,
                                        depth)
                    ) {

                        if (*flagp & NEED_UTF8)
                            FAIL("panic: grok_bslash_N set NEED_UTF8");

                        RETURN_FAIL_ON_RESTART_FLAGP(flagp);

                        if (cp_count < 0) {
                            vFAIL("\\N in a character class must be a named character: \\N{...}");
                        }
                        else if (cp_count == 0) {
                            ckWARNreg(RExC_parse,
                              "Ignoring zero length \\N{} in character class");
                        }
                        else { /* cp_count > 1 */
                            assert(cp_count > 1);
                            if (! RExC_in_multi_char_class) {
                                if ( ! allow_mutiple_chars
                                    || invert
                                    || range
                                    || *RExC_parse == '-')
                                {
                                    if (strict) {
                                        RExC_parse--;
                                        vFAIL("\\N{} in inverted character class or as a range end-point is restricted to one character");
                                    }
                                    ckWARNreg(RExC_parse, "Using just the first character returned by \\N{} in character class");
                                    break; /* <value> contains the first code
                                              point. Drop out of the switch to
                                              process it */
                                }
                                else {
                                    SV * multi_char_N = newSVpvn(backslash_N_beg,
                                                 RExC_parse - backslash_N_beg);
                                    multi_char_matches
                                        = add_multi_match(multi_char_matches,
                                                          multi_char_N,
                                                          cp_count);
                                }
                            }
                        } /* End of cp_count != 1 */

                        /* This element should not be processed further in this
                         * class */
                        element_count--;
                        value = save_value;
                        prevvalue = save_prevvalue;
                        continue;   /* Back to top of loop to get next char */
                    }

                    /* Here, is a single code point, and <value> contains it */
                    unicode_range = TRUE;   /* \N{} are Unicode */
                }
                break;
	    case 'p':
	    case 'P':
		{
		char *e;

		/* \p means they want Unicode semantics */
		REQUIRE_UNI_RULES(flagp, 0);

		if (RExC_parse >= RExC_end)
		    vFAIL2("Empty \\%c", (U8)value);
		if (*RExC_parse == '{') {
		    const U8 c = (U8)value;
		    e = (char *) memchr(RExC_parse, '}', RExC_end - RExC_parse);
                    if (!e) {
                        RExC_parse++;
                        vFAIL2("Missing right brace on \\%c{}", c);
                    }

                    RExC_parse++;

                    /* White space is allowed adjacent to the braces and after
                     * any '^', even when not under /x */
                    while (isSPACE(*RExC_parse)) {
                         RExC_parse++;
		    }

		    if (UCHARAT(RExC_parse) == '^') {

                        /* toggle.  (The rhs xor gets the single bit that
                         * differs between P and p; the other xor inverts just
                         * that bit) */
                        value ^= 'P' ^ 'p';

                        RExC_parse++;
                        while (isSPACE(*RExC_parse)) {
                            RExC_parse++;
                        }
                    }

                    if (e == RExC_parse)
                        vFAIL2("Empty \\%c{}", c);

		    n = e - RExC_parse;
		    while (isSPACE(*(RExC_parse + n - 1)))
		        n--;

		}   /* The \p isn't immediately followed by a '{' */
		else if (! isALPHA(*RExC_parse)) {
                    RExC_parse += (UTF)
                                  ? UTF8_SAFE_SKIP(RExC_parse, RExC_end)
                                  : 1;
                    vFAIL2("Character following \\%c must be '{' or a "
                           "single-character Unicode property name",
                           (U8) value);
                }
                else {
		    e = RExC_parse;
		    n = 1;
		}
		{
                    char* name = RExC_parse;

                    /* Any message returned about expanding the definition */
                    SV* msg = newSVpvs_flags("", SVs_TEMP);

                    /* If set TRUE, the property is user-defined as opposed to
                     * official Unicode */
                    bool user_defined = FALSE;

                    SV * prop_definition = parse_uniprop_string(
                                            name, n, UTF, FOLD,
                                            FALSE, /* This is compile-time */

                                            /* We can't defer this defn when
                                             * the full result is required in
                                             * this call */
                                            ! cBOOL(ret_invlist),

                                            &user_defined,
                                            msg,
                                            0 /* Base level */
                                           );
                    if (SvCUR(msg)) {   /* Assumes any error causes a msg */
                        assert(prop_definition == NULL);
                        RExC_parse = e + 1;
                        if (SvUTF8(msg)) {  /* msg being UTF-8 makes the whole
                                               thing so, or else the display is
                                               mojibake */
                            RExC_utf8 = TRUE;
                        }
			/* diag_listed_as: Can't find Unicode property definition "%s" in regex; marked by <-- HERE in m/%s/ */
                        vFAIL2utf8f("%" UTF8f, UTF8fARG(SvUTF8(msg),
                                    SvCUR(msg), SvPVX(msg)));
                    }

                    if (! is_invlist(prop_definition)) {

                        /* Here, the definition isn't known, so we have gotten
                         * returned a string that will be evaluated if and when
                         * encountered at runtime.  We add it to the list of
                         * such properties, along with whether it should be
                         * complemented or not */
                        if (value == 'P') {
                            sv_catpvs(listsv, "!");
                        }
                        else {
                            sv_catpvs(listsv, "+");
                        }
                        sv_catsv(listsv, prop_definition);

                        has_runtime_dependency |= HAS_USER_DEFINED_PROPERTY;

                        /* We don't know yet what this matches, so have to flag
                         * it */
                        anyof_flags |= ANYOF_SHARED_d_UPPER_LATIN1_UTF8_STRING_MATCHES_non_d_RUNTIME_USER_PROP;
                    }
                    else {
                        assert (prop_definition && is_invlist(prop_definition));

                        /* Here we do have the complete property definition
                         *
                         * Temporary workaround for [perl #133136].  For this
                         * precise input that is in the .t that is failing,
                         * load utf8.pm, which is what the test wants, so that
                         * that .t passes */
                        if (     memEQs(RExC_start, e + 1 - RExC_start,
                                        "foo\\p{Alnum}")
                            && ! hv_common(GvHVn(PL_incgv),
                                           NULL,
                                           "utf8.pm", sizeof("utf8.pm") - 1,
                                           0, HV_FETCH_ISEXISTS, NULL, 0))
                        {
                            require_pv("utf8.pm");
                        }

                        if (! user_defined &&
                            /* We warn on matching an above-Unicode code point
                             * if the match would return true, except don't
                             * warn for \p{All}, which has exactly one element
                             * = 0 */
                            (_invlist_contains_cp(prop_definition, 0x110000)
                                && (! (_invlist_len(prop_definition) == 1
                                       && *invlist_array(prop_definition) == 0))))
                        {
                            warn_super = TRUE;
                        }

                        /* Invert if asking for the complement */
                        if (value == 'P') {
			    _invlist_union_complement_2nd(properties,
                                                          prop_definition,
                                                          &properties);
                        }
                        else {
                            _invlist_union(properties, prop_definition, &properties);
			}
                    }
                }

		RExC_parse = e + 1;
                namedclass = ANYOF_UNIPROP;  /* no official name, but it's
                                                named */
		}
		break;
	    case 'n':	value = '\n';			break;
	    case 'r':	value = '\r';			break;
	    case 't':	value = '\t';			break;
	    case 'f':	value = '\f';			break;
	    case 'b':	value = '\b';			break;
	    case 'e':	value = ESC_NATIVE;             break;
	    case 'a':	value = '\a';                   break;
	    case 'o':
		RExC_parse--;	/* function expects to be pointed at the 'o' */
		{
		    const char* error_msg;
		    bool valid = grok_bslash_o(&RExC_parse,
                                               RExC_end,
					       &value,
					       &error_msg,
                                               TO_OUTPUT_WARNINGS(RExC_parse),
                                               strict,
                                               silence_non_portable,
                                               UTF);
		    if (! valid) {
			vFAIL(error_msg);
		    }
                    UPDATE_WARNINGS_LOC(RExC_parse - 1);
		}
                non_portable_endpoint++;
		break;
	    case 'x':
		RExC_parse--;	/* function expects to be pointed at the 'x' */
		{
		    const char* error_msg;
		    bool valid = grok_bslash_x(&RExC_parse,
                                               RExC_end,
					       &value,
					       &error_msg,
					       TO_OUTPUT_WARNINGS(RExC_parse),
                                               strict,
                                               silence_non_portable,
                                               UTF);
                    if (! valid) {
			vFAIL(error_msg);
		    }
                    UPDATE_WARNINGS_LOC(RExC_parse - 1);
		}
                non_portable_endpoint++;
		break;
	    case 'c':
		value = grok_bslash_c(*RExC_parse, TO_OUTPUT_WARNINGS(RExC_parse));
                UPDATE_WARNINGS_LOC(RExC_parse);
		RExC_parse++;
                non_portable_endpoint++;
		break;
	    case '0': case '1': case '2': case '3': case '4':
	    case '5': case '6': case '7':
		{
		    /* Take 1-3 octal digits */
		    I32 flags = PERL_SCAN_SILENT_ILLDIGIT;
                    numlen = (strict) ? 4 : 3;
                    value = grok_oct(--RExC_parse, &numlen, &flags, NULL);
		    RExC_parse += numlen;
                    if (numlen != 3) {
                        if (strict) {
                            RExC_parse += (UTF)
                                          ? UTF8_SAFE_SKIP(RExC_parse, RExC_end)
                                          : 1;
                            vFAIL("Need exactly 3 octal digits");
                        }
                        else if (   numlen < 3 /* like \08, \178 */
                                 && RExC_parse < RExC_end
                                 && isDIGIT(*RExC_parse)
                                 && ckWARN(WARN_REGEXP))
                        {
                            reg_warn_non_literal_string(
                                 RExC_parse + 1,
                                 form_short_octal_warning(RExC_parse, numlen));
                        }
                    }
                    non_portable_endpoint++;
		    break;
		}
	    default:
		/* Allow \_ to not give an error */
		if (isWORDCHAR(value) && value != '_') {
                    if (strict) {
                        vFAIL2("Unrecognized escape \\%c in character class",
                               (int)value);
                    }
                    else {
                        ckWARN2reg(RExC_parse,
                            "Unrecognized escape \\%c in character class passed through",
                            (int)value);
                    }
		}
		break;
	    }   /* End of switch on char following backslash */
	} /* end of handling backslash escape sequences */

        /* Here, we have the current token in 'value' */

	if (namedclass > OOB_NAMEDCLASS) { /* this is a named class \blah */
            U8 classnum;

	    /* a bad range like a-\d, a-[:digit:].  The '-' is taken as a
	     * literal, as is the character that began the false range, i.e.
	     * the 'a' in the examples */
	    if (range) {
                const int w = (RExC_parse >= rangebegin)
                                ? RExC_parse - rangebegin
                                : 0;
                if (strict) {
                    vFAIL2utf8f(
                        "False [] range \"%" UTF8f "\"",
                        UTF8fARG(UTF, w, rangebegin));
                }
                else {
                    ckWARN2reg(RExC_parse,
                        "False [] range \"%" UTF8f "\"",
                        UTF8fARG(UTF, w, rangebegin));
                    cp_list = add_cp_to_invlist(cp_list, '-');
                    cp_foldable_list = add_cp_to_invlist(cp_foldable_list,
                                                            prevvalue);
                }

		range = 0; /* this was not a true range */
                element_count += 2; /* So counts for three values */
	    }

            classnum = namedclass_to_classnum(namedclass);

	    if (LOC && namedclass < ANYOF_POSIXL_MAX
#ifndef HAS_ISASCII
                && classnum != _CC_ASCII
#endif
            ) {
                SV* scratch_list = NULL;

                /* What the Posix classes (like \w, [:space:]) match isn't
                 * generally knowable under locale until actual match time.  A
                 * special node is used for these which has extra space for a
                 * bitmap, with a bit reserved for each named class that is to
                 * be matched against.  (This isn't needed for \p{} and
                 * pseudo-classes, as they are not affected by locale, and
                 * hence are dealt with separately.)  However, if a named class
                 * and its complement are both present, then it matches
                 * everything, and there is no runtime dependency.  Odd numbers
                 * are the complements of the next lower number, so xor works.
                 * (Note that something like [\w\D] should match everything,
                 * because \d should be a proper subset of \w.  But rather than
                 * trust that the locale is well behaved, we leave this to
                 * runtime to sort out) */
                if (POSIXL_TEST(posixl, namedclass ^ 1)) {
                    cp_list = _add_range_to_invlist(cp_list, 0, UV_MAX);
                    POSIXL_ZERO(posixl);
                    has_runtime_dependency &= ~HAS_L_RUNTIME_DEPENDENCY;
                    anyof_flags &= ~ANYOF_MATCHES_POSIXL;
                    continue;   /* We could ignore the rest of the class, but
                                   best to parse it for any errors */
                }
                else { /* Here, isn't the complement of any already parsed
                          class */
                    POSIXL_SET(posixl, namedclass);
                    has_runtime_dependency |= HAS_L_RUNTIME_DEPENDENCY;
                    anyof_flags |= ANYOF_MATCHES_POSIXL;

                    /* The above-Latin1 characters are not subject to locale
                     * rules.  Just add them to the unconditionally-matched
                     * list */

                    /* Get the list of the above-Latin1 code points this
                     * matches */
                    _invlist_intersection_maybe_complement_2nd(PL_AboveLatin1,
                                            PL_XPosix_ptrs[classnum],

                                            /* Odd numbers are complements,
                                             * like NDIGIT, NASCII, ... */
                                            namedclass % 2 != 0,
                                            &scratch_list);
                    /* Checking if 'cp_list' is NULL first saves an extra
                     * clone.  Its reference count will be decremented at the
                     * next union, etc, or if this is the only instance, at the
                     * end of the routine */
                    if (! cp_list) {
                        cp_list = scratch_list;
                    }
                    else {
                        _invlist_union(cp_list, scratch_list, &cp_list);
                        SvREFCNT_dec_NN(scratch_list);
                    }
                    continue;   /* Go get next character */
                }
            }
            else {

                /* Here, is not /l, or is a POSIX class for which /l doesn't
                 * matter (or is a Unicode property, which is skipped here). */
                if (namedclass >= ANYOF_POSIXL_MAX) {  /* If a special class */
                    if (namedclass != ANYOF_UNIPROP) { /* UNIPROP = \p and \P */

                        /* Here, should be \h, \H, \v, or \V.  None of /d, /i
                         * nor /l make a difference in what these match,
                         * therefore we just add what they match to cp_list. */
                        if (classnum != _CC_VERTSPACE) {
                            assert(   namedclass == ANYOF_HORIZWS
                                   || namedclass == ANYOF_NHORIZWS);

                            /* It turns out that \h is just a synonym for
                             * XPosixBlank */
                            classnum = _CC_BLANK;
                        }

                        _invlist_union_maybe_complement_2nd(
                                cp_list,
                                PL_XPosix_ptrs[classnum],
                                namedclass % 2 != 0,    /* Complement if odd
                                                          (NHORIZWS, NVERTWS)
                                                        */
                                &cp_list);
                    }
                }
                else if (   AT_LEAST_UNI_SEMANTICS
                         || classnum == _CC_ASCII
                         || (DEPENDS_SEMANTICS && (   classnum == _CC_DIGIT
                                                   || classnum == _CC_XDIGIT)))
                {
                    /* We usually have to worry about /d affecting what POSIX
                     * classes match, with special code needed because we won't
                     * know until runtime what all matches.  But there is no
                     * extra work needed under /u and /a; and [:ascii:] is
                     * unaffected by /d; and :digit: and :xdigit: don't have
                     * runtime differences under /d.  So we can special case
                     * these, and avoid some extra work below, and at runtime.
                     * */
                    _invlist_union_maybe_complement_2nd(
                                                     simple_posixes,
                                                      ((AT_LEAST_ASCII_RESTRICTED)
                                                       ? PL_Posix_ptrs[classnum]
                                                       : PL_XPosix_ptrs[classnum]),
                                                     namedclass % 2 != 0,
                                                     &simple_posixes);
                }
                else {  /* Garden variety class.  If is NUPPER, NALPHA, ...
                           complement and use nposixes */
                    SV** posixes_ptr = namedclass % 2 == 0
                                       ? &posixes
                                       : &nposixes;
                    _invlist_union_maybe_complement_2nd(
                                                     *posixes_ptr,
                                                     PL_XPosix_ptrs[classnum],
                                                     namedclass % 2 != 0,
                                                     posixes_ptr);
                }
	    }
	} /* end of namedclass \blah */

        SKIP_BRACKETED_WHITE_SPACE(skip_white, RExC_parse);

        /* If 'range' is set, 'value' is the ending of a range--check its
         * validity.  (If value isn't a single code point in the case of a
         * range, we should have figured that out above in the code that
         * catches false ranges).  Later, we will handle each individual code
         * point in the range.  If 'range' isn't set, this could be the
         * beginning of a range, so check for that by looking ahead to see if
         * the next real character to be processed is the range indicator--the
         * minus sign */

	if (range) {
#ifdef EBCDIC
            /* For unicode ranges, we have to test that the Unicode as opposed
             * to the native values are not decreasing.  (Above 255, there is
             * no difference between native and Unicode) */
	    if (unicode_range && prevvalue < 255 && value < 255) {
                if (NATIVE_TO_LATIN1(prevvalue) > NATIVE_TO_LATIN1(value)) {
                    goto backwards_range;
                }
            }
            else
#endif
	    if (prevvalue > value) /* b-a */ {
		int w;
#ifdef EBCDIC
              backwards_range:
#endif
                w = RExC_parse - rangebegin;
                vFAIL2utf8f(
                    "Invalid [] range \"%" UTF8f "\"",
                    UTF8fARG(UTF, w, rangebegin));
                NOT_REACHED; /* NOTREACHED */
	    }
	}
	else {
            prevvalue = value; /* save the beginning of the potential range */
            if (! stop_at_1     /* Can't be a range if parsing just one thing */
                && *RExC_parse == '-')
            {
                char* next_char_ptr = RExC_parse + 1;

                /* Get the next real char after the '-' */
                SKIP_BRACKETED_WHITE_SPACE(skip_white, next_char_ptr);

                /* If the '-' is at the end of the class (just before the ']',
                 * it is a literal minus; otherwise it is a range */
                if (next_char_ptr < RExC_end && *next_char_ptr != ']') {
                    RExC_parse = next_char_ptr;

                    /* a bad range like \w-, [:word:]- ? */
                    if (namedclass > OOB_NAMEDCLASS) {
                        if (strict || ckWARN(WARN_REGEXP)) {
                            const int w = RExC_parse >= rangebegin
                                          ?  RExC_parse - rangebegin
                                          : 0;
                            if (strict) {
                                vFAIL4("False [] range \"%*.*s\"",
                                    w, w, rangebegin);
                            }
                            else {
                                vWARN4(RExC_parse,
                                    "False [] range \"%*.*s\"",
                                    w, w, rangebegin);
                            }
                        }
                        cp_list = add_cp_to_invlist(cp_list, '-');
                        element_count++;
                    } else
                        range = 1;	/* yeah, it's a range! */
                    continue;	/* but do it the next time */
                }
	    }
	}

        if (namedclass > OOB_NAMEDCLASS) {
            continue;
        }

        /* Here, we have a single value this time through the loop, and
         * <prevvalue> is the beginning of the range, if any; or <value> if
         * not. */

	/* non-Latin1 code point implies unicode semantics. */
	if (value > 255) {
            REQUIRE_UNI_RULES(flagp, 0);
	}

        /* Ready to process either the single value, or the completed range.
         * For single-valued non-inverted ranges, we consider the possibility
         * of multi-char folds.  (We made a conscious decision to not do this
         * for the other cases because it can often lead to non-intuitive
         * results.  For example, you have the peculiar case that:
         *  "s s" =~ /^[^\xDF]+$/i => Y
         *  "ss"  =~ /^[^\xDF]+$/i => N
         *
         * See [perl #89750] */
        if (FOLD && allow_mutiple_chars && value == prevvalue) {
            if (    value == LATIN_SMALL_LETTER_SHARP_S
                || (value > 255 && _invlist_contains_cp(PL_HasMultiCharFold,
                                                        value)))
            {
                /* Here <value> is indeed a multi-char fold.  Get what it is */

                U8 foldbuf[UTF8_MAXBYTES_CASE+1];
                STRLEN foldlen;

                UV folded = _to_uni_fold_flags(
                                value,
                                foldbuf,
                                &foldlen,
                                FOLD_FLAGS_FULL | (ASCII_FOLD_RESTRICTED
                                                   ? FOLD_FLAGS_NOMIX_ASCII
                                                   : 0)
                                );

                /* Here, <folded> should be the first character of the
                 * multi-char fold of <value>, with <foldbuf> containing the
                 * whole thing.  But, if this fold is not allowed (because of
                 * the flags), <fold> will be the same as <value>, and should
                 * be processed like any other character, so skip the special
                 * handling */
                if (folded != value) {

                    /* Skip if we are recursed, currently parsing the class
                     * again.  Otherwise add this character to the list of
                     * multi-char folds. */
                    if (! RExC_in_multi_char_class) {
                        STRLEN cp_count = utf8_length(foldbuf,
                                                      foldbuf + foldlen);
                        SV* multi_fold = sv_2mortal(newSVpvs(""));

                        Perl_sv_catpvf(aTHX_ multi_fold, "\\x{%" UVXf "}", value);

                        multi_char_matches
                                        = add_multi_match(multi_char_matches,
                                                          multi_fold,
                                                          cp_count);

                    }

                    /* This element should not be processed further in this
                     * class */
                    element_count--;
                    value = save_value;
                    prevvalue = save_prevvalue;
                    continue;
                }
            }
        }

        if (strict && ckWARN(WARN_REGEXP)) {
            if (range) {

                /* If the range starts above 255, everything is portable and
                 * likely to be so for any forseeable character set, so don't
                 * warn. */
                if (unicode_range && non_portable_endpoint && prevvalue < 256) {
                    vWARN(RExC_parse, "Both or neither range ends should be Unicode");
                }
                else if (prevvalue != value) {

                    /* Under strict, ranges that stop and/or end in an ASCII
                     * printable should have each end point be a portable value
                     * for it (preferably like 'A', but we don't warn if it is
                     * a (portable) Unicode name or code point), and the range
                     * must be be all digits or all letters of the same case.
                     * Otherwise, the range is non-portable and unclear as to
                     * what it contains */
                    if (             (isPRINT_A(prevvalue) || isPRINT_A(value))
                        && (          non_portable_endpoint
                            || ! (   (isDIGIT_A(prevvalue) && isDIGIT_A(value))
                                  || (isLOWER_A(prevvalue) && isLOWER_A(value))
                                  || (isUPPER_A(prevvalue) && isUPPER_A(value))
                    ))) {
                        vWARN(RExC_parse, "Ranges of ASCII printables should"
                                          " be some subset of \"0-9\","
                                          " \"A-Z\", or \"a-z\"");
                    }
                    else if (prevvalue >= FIRST_NON_ASCII_DECIMAL_DIGIT) {
                        SSize_t index_start;
                        SSize_t index_final;

                        /* But the nature of Unicode and languages mean we
                         * can't do the same checks for above-ASCII ranges,
                         * except in the case of digit ones.  These should
                         * contain only digits from the same group of 10.  The
                         * ASCII case is handled just above.  Hence here, the
                         * range could be a range of digits.  First some
                         * unlikely special cases.  Grandfather in that a range
                         * ending in 19DA (NEW TAI LUE THAM DIGIT ONE) is bad
                         * if its starting value is one of the 10 digits prior
                         * to it.  This is because it is an alternate way of
                         * writing 19D1, and some people may expect it to be in
                         * that group.  But it is bad, because it won't give
                         * the expected results.  In Unicode 5.2 it was
                         * considered to be in that group (of 11, hence), but
                         * this was fixed in the next version */

                        if (UNLIKELY(value == 0x19DA && prevvalue >= 0x19D0)) {
                            goto warn_bad_digit_range;
                        }
                        else if (UNLIKELY(   prevvalue >= 0x1D7CE
                                          &&     value <= 0x1D7FF))
                        {
                            /* This is the only other case currently in Unicode
                             * where the algorithm below fails.  The code
                             * points just above are the end points of a single
                             * range containing only decimal digits.  It is 5
                             * different series of 0-9.  All other ranges of
                             * digits currently in Unicode are just a single
                             * series.  (And mktables will notify us if a later
                             * Unicode version breaks this.)
                             *
                             * If the range being checked is at most 9 long,
                             * and the digit values represented are in
                             * numerical order, they are from the same series.
                             * */
                            if (         value - prevvalue > 9
                                ||    (((    value - 0x1D7CE) % 10)
                                     <= (prevvalue - 0x1D7CE) % 10))
                            {
                                goto warn_bad_digit_range;
                            }
                        }
                        else {

                            /* For all other ranges of digits in Unicode, the
                             * algorithm is just to check if both end points
                             * are in the same series, which is the same range.
                             * */
                            index_start = _invlist_search(
                                                    PL_XPosix_ptrs[_CC_DIGIT],
                                                    prevvalue);

                            /* Warn if the range starts and ends with a digit,
                             * and they are not in the same group of 10. */
                            if (   index_start >= 0
                                && ELEMENT_RANGE_MATCHES_INVLIST(index_start)
                                && (index_final =
                                    _invlist_search(PL_XPosix_ptrs[_CC_DIGIT],
                                                    value)) != index_start
                                && index_final >= 0
                                && ELEMENT_RANGE_MATCHES_INVLIST(index_final))
                            {
                              warn_bad_digit_range:
                                vWARN(RExC_parse, "Ranges of digits should be"
                                                  " from the same group of"
                                                  " 10");
                            }
                        }
                    }
                }
            }
            if ((! range || prevvalue == value) && non_portable_endpoint) {
                if (isPRINT_A(value)) {
                    char literal[3];
                    unsigned d = 0;
                    if (isBACKSLASHED_PUNCT(value)) {
                        literal[d++] = '\\';
                    }
                    literal[d++] = (char) value;
                    literal[d++] = '\0';

                    vWARN4(RExC_parse,
                           "\"%.*s\" is more clearly written simply as \"%s\"",
                           (int) (RExC_parse - rangebegin),
                           rangebegin,
                           literal
                        );
                }
                else if isMNEMONIC_CNTRL(value) {
                    vWARN4(RExC_parse,
                           "\"%.*s\" is more clearly written simply as \"%s\"",
                           (int) (RExC_parse - rangebegin),
                           rangebegin,
                           cntrl_to_mnemonic((U8) value)
                        );
                }
            }
        }

        /* Deal with this element of the class */

#ifndef EBCDIC
        cp_foldable_list = _add_range_to_invlist(cp_foldable_list,
                                                    prevvalue, value);
#else
        /* On non-ASCII platforms, for ranges that span all of 0..255, and ones
         * that don't require special handling, we can just add the range like
         * we do for ASCII platforms */
        if ((UNLIKELY(prevvalue == 0) && value >= 255)
            || ! (prevvalue < 256
                    && (unicode_range
                        || (! non_portable_endpoint
                            && ((isLOWER_A(prevvalue) && isLOWER_A(value))
                                || (isUPPER_A(prevvalue)
                                    && isUPPER_A(value)))))))
        {
            cp_foldable_list = _add_range_to_invlist(cp_foldable_list,
                                                        prevvalue, value);
        }
        else {
            /* Here, requires special handling.  This can be because it is a
             * range whose code points are considered to be Unicode, and so
             * must be individually translated into native, or because its a
             * subrange of 'A-Z' or 'a-z' which each aren't contiguous in
             * EBCDIC, but we have defined them to include only the "expected"
             * upper or lower case ASCII alphabetics.  Subranges above 255 are
             * the same in native and Unicode, so can be added as a range */
            U8 start = NATIVE_TO_LATIN1(prevvalue);
            unsigned j;
            U8 end = (value < 256) ? NATIVE_TO_LATIN1(value) : 255;
            for (j = start; j <= end; j++) {
                cp_foldable_list = add_cp_to_invlist(cp_foldable_list, LATIN1_TO_NATIVE(j));
            }
            if (value > 255) {
                cp_foldable_list = _add_range_to_invlist(cp_foldable_list,
                                                            256, value);
            }
        }
#endif

	range = 0; /* this range (if it was one) is done now */
    } /* End of loop through all the text within the brackets */

    if (   posix_warnings && av_tindex_skip_len_mg(posix_warnings) >= 0) {
        output_posix_warnings(pRExC_state, posix_warnings);
    }

    /* If anything in the class expands to more than one character, we have to
     * deal with them by building up a substitute parse string, and recursively
     * calling reg() on it, instead of proceeding */
    if (multi_char_matches) {
	SV * substitute_parse = newSVpvn_flags("?:", 2, SVs_TEMP);
        I32 cp_count;
	STRLEN len;
	char *save_end = RExC_end;
	char *save_parse = RExC_parse;
	char *save_start = RExC_start;
        Size_t constructed_prefix_len = 0; /* This gives the length of the
                                              constructed portion of the
                                              substitute parse. */
        bool first_time = TRUE;     /* First multi-char occurrence doesn't get
                                       a "|" */
        I32 reg_flags;

        assert(! invert);
        /* Only one level of recursion allowed */
        assert(RExC_copy_start_in_constructed == RExC_precomp);

#if 0   /* Have decided not to deal with multi-char folds in inverted classes,
           because too confusing */
        if (invert) {
            sv_catpvs(substitute_parse, "(?:");
        }
#endif

        /* Look at the longest folds first */
        for (cp_count = av_tindex_skip_len_mg(multi_char_matches);
                        cp_count > 0;
                        cp_count--)
        {

            if (av_exists(multi_char_matches, cp_count)) {
                AV** this_array_ptr;
                SV* this_sequence;

                this_array_ptr = (AV**) av_fetch(multi_char_matches,
                                                 cp_count, FALSE);
                while ((this_sequence = av_pop(*this_array_ptr)) !=
                                                                &PL_sv_undef)
                {
                    if (! first_time) {
                        sv_catpvs(substitute_parse, "|");
                    }
                    first_time = FALSE;

                    sv_catpv(substitute_parse, SvPVX(this_sequence));
                }
            }
        }

        /* If the character class contains anything else besides these
         * multi-character folds, have to include it in recursive parsing */
        if (element_count) {
            sv_catpvs(substitute_parse, "|[");
            constructed_prefix_len = SvCUR(substitute_parse);
            sv_catpvn(substitute_parse, orig_parse, RExC_parse - orig_parse);

            /* Put in a closing ']' only if not going off the end, as otherwise
             * we are adding something that really isn't there */
            if (RExC_parse < RExC_end) {
                sv_catpvs(substitute_parse, "]");
            }
        }

        sv_catpvs(substitute_parse, ")");
#if 0
        if (invert) {
            /* This is a way to get the parse to skip forward a whole named
             * sequence instead of matching the 2nd character when it fails the
             * first */
            sv_catpvs(substitute_parse, "(*THEN)(*SKIP)(*FAIL)|.)");
        }
#endif

        /* Set up the data structure so that any errors will be properly
         * reported.  See the comments at the definition of
         * REPORT_LOCATION_ARGS for details */
        RExC_copy_start_in_input = (char *) orig_parse;
	RExC_start = RExC_parse = SvPV(substitute_parse, len);
        RExC_copy_start_in_constructed = RExC_start + constructed_prefix_len;
	RExC_end = RExC_parse + len;
        RExC_in_multi_char_class = 1;

	ret = reg(pRExC_state, 1, &reg_flags, depth+1);

        *flagp |= reg_flags & (HASWIDTH|SIMPLE|SPSTART|POSTPONED|RESTART_PARSE|NEED_UTF8);

        /* And restore so can parse the rest of the pattern */
        RExC_parse = save_parse;
	RExC_start = RExC_copy_start_in_constructed = RExC_copy_start_in_input = save_start;
	RExC_end = save_end;
	RExC_in_multi_char_class = 0;
        SvREFCNT_dec_NN(multi_char_matches);
        return ret;
    }

    /* If folding, we calculate all characters that could fold to or from the
     * ones already on the list */
    if (cp_foldable_list) {
        if (FOLD) {
            UV start, end;	/* End points of code point ranges */

            SV* fold_intersection = NULL;
            SV** use_list;

            /* Our calculated list will be for Unicode rules.  For locale
             * matching, we have to keep a separate list that is consulted at
             * runtime only when the locale indicates Unicode rules (and we
             * don't include potential matches in the ASCII/Latin1 range, as
             * any code point could fold to any other, based on the run-time
             * locale).   For non-locale, we just use the general list */
            if (LOC) {
                use_list = &only_utf8_locale_list;
            }
            else {
                use_list = &cp_list;
            }

            /* Only the characters in this class that participate in folds need
             * be checked.  Get the intersection of this class and all the
             * possible characters that are foldable.  This can quickly narrow
             * down a large class */
            _invlist_intersection(PL_in_some_fold, cp_foldable_list,
                                  &fold_intersection);

            /* Now look at the foldable characters in this class individually */
            invlist_iterinit(fold_intersection);
            while (invlist_iternext(fold_intersection, &start, &end)) {
                UV j;
                UV folded;

                /* Look at every character in the range */
                for (j = start; j <= end; j++) {
                    U8 foldbuf[UTF8_MAXBYTES_CASE+1];
                    STRLEN foldlen;
                    unsigned int k;
                    Size_t folds_count;
                    unsigned int first_fold;
                    const unsigned int * remaining_folds;

                    if (j < 256) {

                        /* Under /l, we don't know what code points below 256
                         * fold to, except we do know the MICRO SIGN folds to
                         * an above-255 character if the locale is UTF-8, so we
                         * add it to the special list (in *use_list)  Otherwise
                         * we know now what things can match, though some folds
                         * are valid under /d only if the target is UTF-8.
                         * Those go in a separate list */
                        if (      IS_IN_SOME_FOLD_L1(j)
                            && ! (LOC && j != MICRO_SIGN))
                        {

                            /* ASCII is always matched; non-ASCII is matched
                             * only under Unicode rules (which could happen
                             * under /l if the locale is a UTF-8 one */
                            if (isASCII(j) || ! DEPENDS_SEMANTICS) {
                                *use_list = add_cp_to_invlist(*use_list,
                                                            PL_fold_latin1[j]);
                            }
                            else if (j != PL_fold_latin1[j]) {
                                upper_latin1_only_utf8_matches
                                        = add_cp_to_invlist(
                                                upper_latin1_only_utf8_matches,
                                                PL_fold_latin1[j]);
                            }
                        }

                        if (HAS_NONLATIN1_SIMPLE_FOLD_CLOSURE(j)
                            && (! isASCII(j) || ! ASCII_FOLD_RESTRICTED))
                        {
                            add_above_Latin1_folds(pRExC_state,
                                                   (U8) j,
                                                   use_list);
                        }
                        continue;
                    }

                    /* Here is an above Latin1 character.  We don't have the
                     * rules hard-coded for it.  First, get its fold.  This is
                     * the simple fold, as the multi-character folds have been
                     * handled earlier and separated out */
                    folded = _to_uni_fold_flags(j, foldbuf, &foldlen,
                                                        (ASCII_FOLD_RESTRICTED)
                                                        ? FOLD_FLAGS_NOMIX_ASCII
                                                        : 0);

                    /* Single character fold of above Latin1.  Add everything
                     * in its fold closure to the list that this node should
                     * match. */
                    folds_count = _inverse_folds(folded, &first_fold,
                                                    &remaining_folds);
                    for (k = 0; k <= folds_count; k++) {
                        UV c = (k == 0)     /* First time through use itself */
                                ? folded
                                : (k == 1)  /* 2nd time use, the first fold */
                                   ? first_fold

                                     /* Then the remaining ones */
                                   : remaining_folds[k-2];

                        /* /aa doesn't allow folds between ASCII and non- */
                        if ((   ASCII_FOLD_RESTRICTED
                            && (isASCII(c) != isASCII(j))))
                        {
                            continue;
                        }

                        /* Folds under /l which cross the 255/256 boundary are
                         * added to a separate list.  (These are valid only
                         * when the locale is UTF-8.) */
                        if (c < 256 && LOC) {
                            *use_list = add_cp_to_invlist(*use_list, c);
                            continue;
                        }

                        if (isASCII(c) || c > 255 || AT_LEAST_UNI_SEMANTICS)
                        {
                            cp_list = add_cp_to_invlist(cp_list, c);
                        }
                        else {
                            /* Similarly folds involving non-ascii Latin1
                             * characters under /d are added to their list */
                            upper_latin1_only_utf8_matches
                                    = add_cp_to_invlist(
                                                upper_latin1_only_utf8_matches,
                                                c);
                        }
                    }
                }
            }
            SvREFCNT_dec_NN(fold_intersection);
        }

        /* Now that we have finished adding all the folds, there is no reason
         * to keep the foldable list separate */
        _invlist_union(cp_list, cp_foldable_list, &cp_list);
	SvREFCNT_dec_NN(cp_foldable_list);
    }

    /* And combine the result (if any) with any inversion lists from posix
     * classes.  The lists are kept separate up to now because we don't want to
     * fold the classes */
    if (simple_posixes) {   /* These are the classes known to be unaffected by
                               /a, /aa, and /d */
        if (cp_list) {
            _invlist_union(cp_list, simple_posixes, &cp_list);
            SvREFCNT_dec_NN(simple_posixes);
        }
        else {
            cp_list = simple_posixes;
        }
    }
    if (posixes || nposixes) {
        if (! DEPENDS_SEMANTICS) {

            /* For everything but /d, we can just add the current 'posixes' and
             * 'nposixes' to the main list */
            if (posixes) {
                if (cp_list) {
                    _invlist_union(cp_list, posixes, &cp_list);
                    SvREFCNT_dec_NN(posixes);
                }
                else {
                    cp_list = posixes;
                }
            }
            if (nposixes) {
                if (cp_list) {
                    _invlist_union(cp_list, nposixes, &cp_list);
                    SvREFCNT_dec_NN(nposixes);
                }
                else {
                    cp_list = nposixes;
                }
            }
        }
        else {
            /* Under /d, things like \w match upper Latin1 characters only if
             * the target string is in UTF-8.  But things like \W match all the
             * upper Latin1 characters if the target string is not in UTF-8.
             *
             * Handle the case with something like \W separately */
            if (nposixes) {
                SV* only_non_utf8_list = invlist_clone(PL_UpperLatin1, NULL);

                /* A complemented posix class matches all upper Latin1
                 * characters if not in UTF-8.  And it matches just certain
                 * ones when in UTF-8.  That means those certain ones are
                 * matched regardless, so can just be added to the
                 * unconditional list */
                if (cp_list) {
                    _invlist_union(cp_list, nposixes, &cp_list);
                    SvREFCNT_dec_NN(nposixes);
                    nposixes = NULL;
                }
                else {
                    cp_list = nposixes;
                }

                /* Likewise for 'posixes' */
                _invlist_union(posixes, cp_list, &cp_list);
                SvREFCNT_dec(posixes);

                /* Likewise for anything else in the range that matched only
                 * under UTF-8 */
                if (upper_latin1_only_utf8_matches) {
                    _invlist_union(cp_list,
                                   upper_latin1_only_utf8_matches,
                                   &cp_list);
                    SvREFCNT_dec_NN(upper_latin1_only_utf8_matches);
                    upper_latin1_only_utf8_matches = NULL;
                }

                /* If we don't match all the upper Latin1 characters regardless
                 * of UTF-8ness, we have to set a flag to match the rest when
                 * not in UTF-8 */
                _invlist_subtract(only_non_utf8_list, cp_list,
                                  &only_non_utf8_list);
                if (_invlist_len(only_non_utf8_list) != 0) {
                    anyof_flags |= ANYOF_SHARED_d_MATCHES_ALL_NON_UTF8_NON_ASCII_non_d_WARN_SUPER;
                }
                SvREFCNT_dec_NN(only_non_utf8_list);
            }
            else {
                /* Here there were no complemented posix classes.  That means
                 * the upper Latin1 characters in 'posixes' match only when the
                 * target string is in UTF-8.  So we have to add them to the
                 * list of those types of code points, while adding the
                 * remainder to the unconditional list.
                 *
                 * First calculate what they are */
                SV* nonascii_but_latin1_properties = NULL;
                _invlist_intersection(posixes, PL_UpperLatin1,
                                      &nonascii_but_latin1_properties);

                /* And add them to the final list of such characters. */
                _invlist_union(upper_latin1_only_utf8_matches,
                               nonascii_but_latin1_properties,
                               &upper_latin1_only_utf8_matches);

                /* Remove them from what now becomes the unconditional list */
                _invlist_subtract(posixes, nonascii_but_latin1_properties,
                                  &posixes);

                /* And add those unconditional ones to the final list */
                if (cp_list) {
                    _invlist_union(cp_list, posixes, &cp_list);
                    SvREFCNT_dec_NN(posixes);
                    posixes = NULL;
                }
                else {
                    cp_list = posixes;
                }

                SvREFCNT_dec(nonascii_but_latin1_properties);

                /* Get rid of any characters from the conditional list that we
                 * now know are matched unconditionally, which may make that
                 * list empty */
                _invlist_subtract(upper_latin1_only_utf8_matches,
                                  cp_list,
                                  &upper_latin1_only_utf8_matches);
                if (_invlist_len(upper_latin1_only_utf8_matches) == 0) {
                    SvREFCNT_dec_NN(upper_latin1_only_utf8_matches);
                    upper_latin1_only_utf8_matches = NULL;
                }
            }
        }
    }

    /* And combine the result (if any) with any inversion list from properties.
     * The lists are kept separate up to now so that we can distinguish the two
     * in regards to matching above-Unicode.  A run-time warning is generated
     * if a Unicode property is matched against a non-Unicode code point. But,
     * we allow user-defined properties to match anything, without any warning,
     * and we also suppress the warning if there is a portion of the character
     * class that isn't a Unicode property, and which matches above Unicode, \W
     * or [\x{110000}] for example.
     * (Note that in this case, unlike the Posix one above, there is no
     * <upper_latin1_only_utf8_matches>, because having a Unicode property
     * forces Unicode semantics */
    if (properties) {
        if (cp_list) {

            /* If it matters to the final outcome, see if a non-property
             * component of the class matches above Unicode.  If so, the
             * warning gets suppressed.  This is true even if just a single
             * such code point is specified, as, though not strictly correct if
             * another such code point is matched against, the fact that they
             * are using above-Unicode code points indicates they should know
             * the issues involved */
            if (warn_super) {
                warn_super = ! (invert
                               ^ (invlist_highest(cp_list) > PERL_UNICODE_MAX));
            }

            _invlist_union(properties, cp_list, &cp_list);
            SvREFCNT_dec_NN(properties);
        }
        else {
            cp_list = properties;
        }

        if (warn_super) {
            anyof_flags
             |= ANYOF_SHARED_d_MATCHES_ALL_NON_UTF8_NON_ASCII_non_d_WARN_SUPER;

            /* Because an ANYOF node is the only one that warns, this node
             * can't be optimized into something else */
            optimizable = FALSE;
        }
    }

    /* Here, we have calculated what code points should be in the character
     * class.
     *
     * Now we can see about various optimizations.  Fold calculation (which we
     * did above) needs to take place before inversion.  Otherwise /[^k]/i
     * would invert to include K, which under /i would match k, which it
     * shouldn't.  Therefore we can't invert folded locale now, as it won't be
     * folded until runtime */

    /* If we didn't do folding, it's because some information isn't available
     * until runtime; set the run-time fold flag for these  We know to set the
     * flag if we have a non-NULL list for UTF-8 locales, or the class matches
     * at least one 0-255 range code point */
    if (LOC && FOLD) {

        /* Some things on the list might be unconditionally included because of
         * other components.  Remove them, and clean up the list if it goes to
         * 0 elements */
        if (only_utf8_locale_list && cp_list) {
            _invlist_subtract(only_utf8_locale_list, cp_list,
                              &only_utf8_locale_list);

            if (_invlist_len(only_utf8_locale_list) == 0) {
                SvREFCNT_dec_NN(only_utf8_locale_list);
                only_utf8_locale_list = NULL;
            }
        }
        if (    only_utf8_locale_list
            || (cp_list && (   _invlist_contains_cp(cp_list, LATIN_CAPITAL_LETTER_I_WITH_DOT_ABOVE)
                            || _invlist_contains_cp(cp_list, LATIN_SMALL_LETTER_DOTLESS_I))))
        {
            has_runtime_dependency |= HAS_L_RUNTIME_DEPENDENCY;
            anyof_flags
                 |= ANYOFL_FOLD
                 |  ANYOFL_SHARED_UTF8_LOCALE_fold_HAS_MATCHES_nonfold_REQD;
        }
        else if (cp_list) { /* Look to see if a 0-255 code point is in list */
            UV start, end;
            invlist_iterinit(cp_list);
            if (invlist_iternext(cp_list, &start, &end) && start < 256) {
                anyof_flags |= ANYOFL_FOLD;
                has_runtime_dependency |= HAS_L_RUNTIME_DEPENDENCY;
            }
            invlist_iterfinish(cp_list);
        }
    }
    else if (   DEPENDS_SEMANTICS
             && (    upper_latin1_only_utf8_matches
                 || (anyof_flags & ANYOF_SHARED_d_MATCHES_ALL_NON_UTF8_NON_ASCII_non_d_WARN_SUPER)))
    {
        RExC_seen_d_op = TRUE;
        has_runtime_dependency |= HAS_D_RUNTIME_DEPENDENCY;
    }

    /* Optimize inverted patterns (e.g. [^a-z]) when everything is known at
     * compile time. */
    if (     cp_list
        &&   invert
        && ! has_runtime_dependency)
    {
        _invlist_invert(cp_list);

	/* Clear the invert flag since have just done it here */
	invert = FALSE;
    }

    if (ret_invlist) {
        *ret_invlist = cp_list;

        return RExC_emit;
    }

    /* All possible optimizations below still have these characteristics.
     * (Multi-char folds aren't SIMPLE, but they don't get this far in this
     * routine) */
    *flagp |= HASWIDTH|SIMPLE;

    if (anyof_flags & ANYOF_LOCALE_FLAGS) {
        RExC_contains_locale = 1;
    }

    /* Some character classes are equivalent to other nodes.  Such nodes take
     * up less room, and some nodes require fewer operations to execute, than
     * ANYOF nodes.  EXACTish nodes may be joinable with adjacent nodes to
     * improve efficiency. */

    if (optimizable) {
        PERL_UINT_FAST8_T i;
        Size_t partial_cp_count = 0;
        UV start[MAX_FOLD_FROMS+1] = { 0 }; /* +1 for the folded-to char */
        UV   end[MAX_FOLD_FROMS+1] = { 0 };

        if (cp_list) { /* Count the code points in enough ranges that we would
                          see all the ones possible in any fold in this version
                          of Unicode */

            invlist_iterinit(cp_list);
            for (i = 0; i <= MAX_FOLD_FROMS; i++) {
                if (! invlist_iternext(cp_list, &start[i], &end[i])) {
                    break;
                }
                partial_cp_count += end[i] - start[i] + 1;
            }

            invlist_iterfinish(cp_list);
        }

        /* If we know at compile time that this matches every possible code
         * point, any run-time dependencies don't matter */
        if (start[0] == 0 && end[0] == UV_MAX) {
            if (invert) {
                ret = reganode(pRExC_state, OPFAIL, 0);
            }
            else {
                ret = reg_node(pRExC_state, SANY);
                MARK_NAUGHTY(1);
            }
            goto not_anyof;
        }

        /* Similarly, for /l posix classes, if both a class and its
         * complement match, any run-time dependencies don't matter */
        if (posixl) {
            for (namedclass = 0; namedclass < ANYOF_POSIXL_MAX;
                                                        namedclass += 2)
            {
                if (   POSIXL_TEST(posixl, namedclass)      /* class */
                    && POSIXL_TEST(posixl, namedclass + 1)) /* its complement */
                {
                    if (invert) {
                        ret = reganode(pRExC_state, OPFAIL, 0);
                    }
                    else {
                        ret = reg_node(pRExC_state, SANY);
                        MARK_NAUGHTY(1);
                    }
                    goto not_anyof;
                }
            }
            /* For well-behaved locales, some classes are subsets of others,
             * so complementing the subset and including the non-complemented
             * superset should match everything, like [\D[:alnum:]], and
             * [[:^alpha:][:alnum:]], but some implementations of locales are
             * buggy, and khw thinks its a bad idea to have optimization change
             * behavior, even if it avoids an OS bug in a given case */

#define isSINGLE_BIT_SET(n) isPOWER_OF_2(n)

            /* If is a single posix /l class, can optimize to just that op.
             * Such a node will not match anything in the Latin1 range, as that
             * is not determinable until runtime, but will match whatever the
             * class does outside that range.  (Note that some classes won't
             * match anything outside the range, like [:ascii:]) */
            if (    isSINGLE_BIT_SET(posixl)
                && (partial_cp_count == 0 || start[0] > 255))
            {
                U8 classnum;
                SV * class_above_latin1 = NULL;
                bool already_inverted;
                bool are_equivalent;

                /* Compute which bit is set, which is the same thing as, e.g.,
                 * ANYOF_CNTRL.  From
                 * https://graphics.stanford.edu/~seander/bithacks.html#IntegerLogDeBruijn
                 * */
                static const int MultiplyDeBruijnBitPosition2[32] =
                    {
                    0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
                    31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
                    };

                namedclass = MultiplyDeBruijnBitPosition2[(posixl
                                                          * 0x077CB531U) >> 27];
                classnum = namedclass_to_classnum(namedclass);

                /* The named classes are such that the inverted number is one
                 * larger than the non-inverted one */
                already_inverted = namedclass
                                 - classnum_to_namedclass(classnum);

                /* Create an inversion list of the official property, inverted
                 * if the constructed node list is inverted, and restricted to
                 * only the above latin1 code points, which are the only ones
                 * known at compile time */
                _invlist_intersection_maybe_complement_2nd(
                                                    PL_AboveLatin1,
                                                    PL_XPosix_ptrs[classnum],
                                                    already_inverted,
                                                    &class_above_latin1);
                are_equivalent = _invlistEQ(class_above_latin1, cp_list,
                                                                        FALSE);
                SvREFCNT_dec_NN(class_above_latin1);

                if (are_equivalent) {

                    /* Resolve the run-time inversion flag with this possibly
                     * inverted class */
                    invert = invert ^ already_inverted;

                    ret = reg_node(pRExC_state,
                                   POSIXL + invert * (NPOSIXL - POSIXL));
                    FLAGS(REGNODE_p(ret)) = classnum;
                    goto not_anyof;
                }
            }
        }

        /* khw can't think of any other possible transformation involving
         * these. */
        if (has_runtime_dependency & HAS_USER_DEFINED_PROPERTY) {
            goto is_anyof;
        }

        if (! has_runtime_dependency) {

            /* If the list is empty, nothing matches.  This happens, for
             * example, when a Unicode property that doesn't match anything is
             * the only element in the character class (perluniprops.pod notes
             * such properties). */
            if (partial_cp_count == 0) {
                if (invert) {
                    ret = reg_node(pRExC_state, SANY);
                }
                else {
                    ret = reganode(pRExC_state, OPFAIL, 0);
                }

                goto not_anyof;
            }

            /* If matches everything but \n */
            if (   start[0] == 0 && end[0] == '\n' - 1
                && start[1] == '\n' + 1 && end[1] == UV_MAX)
            {
                assert (! invert);
                ret = reg_node(pRExC_state, REG_ANY);
                MARK_NAUGHTY(1);
                goto not_anyof;
            }
        }

        /* Next see if can optimize classes that contain just a few code points
         * into an EXACTish node.  The reason to do this is to let the
         * optimizer join this node with adjacent EXACTish ones.
         *
         * An EXACTFish node can be generated even if not under /i, and vice
         * versa.  But care must be taken.  An EXACTFish node has to be such
         * that it only matches precisely the code points in the class, but we
         * want to generate the least restrictive one that does that, to
         * increase the odds of being able to join with an adjacent node.  For
         * example, if the class contains [kK], we have to make it an EXACTFAA
         * node to prevent the KELVIN SIGN from matching.  Whether we are under
         * /i or not is irrelevant in this case.  Less obvious is the pattern
         * qr/[\x{02BC}]n/i.  U+02BC is MODIFIER LETTER APOSTROPHE. That is
         * supposed to match the single character U+0149 LATIN SMALL LETTER N
         * PRECEDED BY APOSTROPHE.  And so even though there is no simple fold
         * that includes \X{02BC}, there is a multi-char fold that does, and so
         * the node generated for it must be an EXACTFish one.  On the other
         * hand qr/:/i should generate a plain EXACT node since the colon
         * participates in no fold whatsoever, and having it EXACT tells the
         * optimizer the target string cannot match unless it has a colon in
         * it.
         *
         * We don't typically generate an EXACTish node if doing so would
         * require changing the pattern to UTF-8, as that affects /d and
         * otherwise is slower.  However, under /i, not changing to UTF-8 can
         * miss some potential multi-character folds.  We calculate the
         * EXACTish node, and then decide if something would be missed if we
         * don't upgrade */
        if (   ! posixl
            && ! invert

                /* Only try if there are no more code points in the class than
                 * in the max possible fold */
            &&   partial_cp_count > 0 && partial_cp_count <= MAX_FOLD_FROMS + 1

            && (start[0] < 256 || UTF || FOLD))
        {
            if (partial_cp_count == 1 && ! upper_latin1_only_utf8_matches)
            {
                /* We can always make a single code point class into an
                 * EXACTish node. */

                if (LOC) {

                    /* Here is /l:  Use EXACTL, except /li indicates EXACTFL,
                     * as that means there is a fold not known until runtime so
                     * shows as only a single code point here. */
                    op = (FOLD) ? EXACTFL : EXACTL;
                }
                else if (! FOLD) { /* Not /l and not /i */
                    op = (start[0] < 256) ? EXACT : EXACT_ONLY8;
                }
                else if (start[0] < 256) { /* /i, not /l, and the code point is
                                              small */

                    /* Under /i, it gets a little tricky.  A code point that
                     * doesn't participate in a fold should be an EXACT node.
                     * We know this one isn't the result of a simple fold, or
                     * there'd be more than one code point in the list, but it
                     * could be part of a multi- character fold.  In that case
                     * we better not create an EXACT node, as we would wrongly
                     * be telling the optimizer that this code point must be in
                     * the target string, and that is wrong.  This is because
                     * if the sequence around this code point forms a
                     * multi-char fold, what needs to be in the string could be
                     * the code point that folds to the sequence.
                     *
                     * This handles the case of below-255 code points, as we
                     * have an easy look up for those.  The next clause handles
                     * the above-256 one */
                    op = IS_IN_SOME_FOLD_L1(start[0])
                         ? EXACTFU
                         : EXACT;
                }
                else {  /* /i, larger code point.  Since we are under /i, and
                           have just this code point, we know that it can't
                           fold to something else, so PL_InMultiCharFold
                           applies to it */
                    op = _invlist_contains_cp(PL_InMultiCharFold,
                                              start[0])
                         ? EXACTFU_ONLY8
                         : EXACT_ONLY8;
                }

                value = start[0];
            }
            else if (  ! (has_runtime_dependency & ~HAS_D_RUNTIME_DEPENDENCY)
                     && _invlist_contains_cp(PL_in_some_fold, start[0]))
            {
                /* Here, the only runtime dependency, if any, is from /d, and
                 * the class matches more than one code point, and the lowest
                 * code point participates in some fold.  It might be that the
                 * other code points are /i equivalent to this one, and hence
                 * they would representable by an EXACTFish node.  Above, we
                 * eliminated classes that contain too many code points to be
                 * EXACTFish, with the test for MAX_FOLD_FROMS
                 *
                 * First, special case the ASCII fold pairs, like 'B' and 'b'.
                 * We do this because we have EXACTFAA at our disposal for the
                 * ASCII range */
                if (partial_cp_count == 2 && isASCII(start[0])) {

                    /* The only ASCII characters that participate in folds are
                     * alphabetics */
                    assert(isALPHA(start[0]));
                    if (   end[0] == start[0]   /* First range is a single
                                                   character, so 2nd exists */
                        && isALPHA_FOLD_EQ(start[0], start[1]))
                    {

                        /* Here, is part of an ASCII fold pair */

                        if (   ASCII_FOLD_RESTRICTED
                            || HAS_NONLATIN1_SIMPLE_FOLD_CLOSURE(start[0]))
                        {
                            /* If the second clause just above was true, it
                             * means we can't be under /i, or else the list
                             * would have included more than this fold pair.
                             * Therefore we have to exclude the possibility of
                             * whatever else it is that folds to these, by
                             * using EXACTFAA */
                            op = EXACTFAA;
                        }
                        else if (HAS_NONLATIN1_FOLD_CLOSURE(start[0])) {

                            /* Here, there's no simple fold that start[0] is part
                             * of, but there is a multi-character one.  If we
                             * are not under /i, we want to exclude that
                             * possibility; if under /i, we want to include it
                             * */
                            op = (FOLD) ? EXACTFU : EXACTFAA;
                        }
                        else {

                            /* Here, the only possible fold start[0] particpates in
                             * is with start[1].  /i or not isn't relevant */
                            op = EXACTFU;
                        }

                        value = toFOLD(start[0]);
                    }
                }
                else if (  ! upper_latin1_only_utf8_matches
                         || (   _invlist_len(upper_latin1_only_utf8_matches)
                                                                          == 2
                             && PL_fold_latin1[
                               invlist_highest(upper_latin1_only_utf8_matches)]
                             == start[0]))
                {
                    /* Here, the smallest character is non-ascii or there are
                     * more than 2 code points matched by this node.  Also, we
                     * either don't have /d UTF-8 dependent matches, or if we
                     * do, they look like they could be a single character that
                     * is the fold of the lowest one in the always-match list.
                     * This test quickly excludes most of the false positives
                     * when there are /d UTF-8 depdendent matches.  These are
                     * like LATIN CAPITAL LETTER A WITH GRAVE matching LATIN
                     * SMALL LETTER A WITH GRAVE iff the target string is
                     * UTF-8.  (We don't have to worry above about exceeding
                     * the array bounds of PL_fold_latin1[] because any code
                     * point in 'upper_latin1_only_utf8_matches' is below 256.)
                     *
                     * EXACTFAA would apply only to pairs (hence exactly 2 code
                     * points) in the ASCII range, so we can't use it here to
                     * artificially restrict the fold domain, so we check if
                     * the class does or does not match some EXACTFish node.
                     * Further, if we aren't under /i, and and the folded-to
                     * character is part of a multi-character fold, we can't do
                     * this optimization, as the sequence around it could be
                     * that multi-character fold, and we don't here know the
                     * context, so we have to assume it is that multi-char
                     * fold, to prevent potential bugs.
                     *
                     * To do the general case, we first find the fold of the
                     * lowest code point (which may be higher than the lowest
                     * one), then find everything that folds to it.  (The data
                     * structure we have only maps from the folded code points,
                     * so we have to do the earlier step.) */

                    Size_t foldlen;
                    U8 foldbuf[UTF8_MAXBYTES_CASE];
                    UV folded = _to_uni_fold_flags(start[0],
                                                        foldbuf, &foldlen, 0);
                    unsigned int first_fold;
                    const unsigned int * remaining_folds;
                    Size_t folds_to_this_cp_count = _inverse_folds(
                                                            folded,
                                                            &first_fold,
                                                            &remaining_folds);
                    Size_t folds_count = folds_to_this_cp_count + 1;
                    SV * fold_list = _new_invlist(folds_count);
                    unsigned int i;

                    /* If there are UTF-8 dependent matches, create a temporary
                     * list of what this node matches, including them. */
                    SV * all_cp_list = NULL;
                    SV ** use_this_list = &cp_list;

                    if (upper_latin1_only_utf8_matches) {
                        all_cp_list = _new_invlist(0);
                        use_this_list = &all_cp_list;
                        _invlist_union(cp_list,
                                       upper_latin1_only_utf8_matches,
                                       use_this_list);
                    }

                    /* Having gotten everything that participates in the fold
                     * containing the lowest code point, we turn that into an
                     * inversion list, making sure everything is included. */
                    fold_list = add_cp_to_invlist(fold_list, start[0]);
                    fold_list = add_cp_to_invlist(fold_list, folded);
                    if (folds_to_this_cp_count > 0) {
                        fold_list = add_cp_to_invlist(fold_list, first_fold);
                        for (i = 0; i + 1 < folds_to_this_cp_count; i++) {
                            fold_list = add_cp_to_invlist(fold_list,
                                                        remaining_folds[i]);
                        }
                    }

                    /* If the fold list is identical to what's in this ANYOF
                     * node, the node can be represented by an EXACTFish one
                     * instead */
                    if (_invlistEQ(*use_this_list, fold_list,
                                   0 /* Don't complement */ )
                    ) {

                        /* But, we have to be careful, as mentioned above.
                         * Just the right sequence of characters could match
                         * this if it is part of a multi-character fold.  That
                         * IS what we want if we are under /i.  But it ISN'T
                         * what we want if not under /i, as it could match when
                         * it shouldn't.  So, when we aren't under /i and this
                         * character participates in a multi-char fold, we
                         * don't optimize into an EXACTFish node.  So, for each
                         * case below we have to check if we are folding
                         * and if not, if it is not part of a multi-char fold.
                         * */
                        if (start[0] > 255) {    /* Highish code point */
                            if (FOLD || ! _invlist_contains_cp(
                                            PL_InMultiCharFold, folded))
                            {
                                op = (LOC)
                                     ? EXACTFLU8
                                     : (ASCII_FOLD_RESTRICTED)
                                       ? EXACTFAA
                                       : EXACTFU_ONLY8;
                                value = folded;
                            }
                        }   /* Below, the lowest code point < 256 */
                        else if (    FOLD
                                 &&  folded == 's'
                                 &&  DEPENDS_SEMANTICS)
                        {   /* An EXACTF node containing a single character
                                's', can be an EXACTFU if it doesn't get
                                joined with an adjacent 's' */
                            op = EXACTFU_S_EDGE;
                            value = folded;
                        }
                        else if (    FOLD
                                || ! HAS_NONLATIN1_FOLD_CLOSURE(start[0]))
                        {
                            if (upper_latin1_only_utf8_matches) {
                                op = EXACTF;

                                /* We can't use the fold, as that only matches
                                 * under UTF-8 */
                                value = start[0];
                            }
                            else if (     UNLIKELY(start[0] == MICRO_SIGN)
                                     && ! UTF)
                            {   /* EXACTFUP is a special node for this
                                   character */
                                op = (ASCII_FOLD_RESTRICTED)
                                     ? EXACTFAA
                                     : EXACTFUP;
                                value = MICRO_SIGN;
                            }
                            else if (     ASCII_FOLD_RESTRICTED
                                     && ! isASCII(start[0]))
                            {   /* For ASCII under /iaa, we can use EXACTFU
                                   below */
                                op = EXACTFAA;
                                value = folded;
                            }
                            else {
                                op = EXACTFU;
                                value = folded;
                            }
                        }
                    }

                    SvREFCNT_dec_NN(fold_list);
                    SvREFCNT_dec(all_cp_list);
                }
            }

            if (op != END) {

                /* Here, we have calculated what EXACTish node we would use.
                 * But we don't use it if it would require converting the
                 * pattern to UTF-8, unless not using it could cause us to miss
                 * some folds (hence be buggy) */

                if (! UTF && value > 255) {
                    SV * in_multis = NULL;

                    assert(FOLD);

                    /* If there is no code point that is part of a multi-char
                     * fold, then there aren't any matches, so we don't do this
                     * optimization.  Otherwise, it could match depending on
                     * the context around us, so we do upgrade */
                    _invlist_intersection(PL_InMultiCharFold, cp_list, &in_multis);
                    if (UNLIKELY(_invlist_len(in_multis) != 0)) {
                        REQUIRE_UTF8(flagp);
                    }
                    else {
                        op = END;
                    }
                }

                if (op != END) {
                    U8 len = (UTF) ? UVCHR_SKIP(value) : 1;

                    ret = regnode_guts(pRExC_state, op, len, "exact");
                    FILL_NODE(ret, op);
                    RExC_emit += 1 + STR_SZ(len);
                    STR_LEN(REGNODE_p(ret)) = len;
                    if (len == 1) {
                        *STRING(REGNODE_p(ret)) = (U8) value;
                    }
                    else {
                        uvchr_to_utf8((U8 *) STRING(REGNODE_p(ret)), value);
                    }
                    goto not_anyof;
                }
            }
        }

        if (! has_runtime_dependency) {

            /* See if this can be turned into an ANYOFM node.  Think about the
             * bit patterns in two different bytes.  In some positions, the
             * bits in each will be 1; and in other positions both will be 0;
             * and in some positions the bit will be 1 in one byte, and 0 in
             * the other.  Let 'n' be the number of positions where the bits
             * differ.  We create a mask which has exactly 'n' 0 bits, each in
             * a position where the two bytes differ.  Now take the set of all
             * bytes that when ANDed with the mask yield the same result.  That
             * set has 2**n elements, and is representable by just two 8 bit
             * numbers: the result and the mask.  Importantly, matching the set
             * can be vectorized by creating a word full of the result bytes,
             * and a word full of the mask bytes, yielding a significant speed
             * up.  Here, see if this node matches such a set.  As a concrete
             * example consider [01], and the byte representing '0' which is
             * 0x30 on ASCII machines.  It has the bits 0011 0000.  Take the
             * mask 1111 1110.  If we AND 0x31 and 0x30 with that mask we get
             * 0x30.  Any other bytes ANDed yield something else.  So [01],
             * which is a common usage, is optimizable into ANYOFM, and can
             * benefit from the speed up.  We can only do this on UTF-8
             * invariant bytes, because they have the same bit patterns under
             * UTF-8 as not. */
            PERL_UINT_FAST8_T inverted = 0;
#ifdef EBCDIC
            const PERL_UINT_FAST8_T max_permissible = 0xFF;
#else
            const PERL_UINT_FAST8_T max_permissible = 0x7F;
#endif
            /* If doesn't fit the criteria for ANYOFM, invert and try again.
             * If that works we will instead later generate an NANYOFM, and
             * invert back when through */
            if (invlist_highest(cp_list) > max_permissible) {
                _invlist_invert(cp_list);
                inverted = 1;
            }

            if (invlist_highest(cp_list) <= max_permissible) {
                UV this_start, this_end;
                UV lowest_cp = UV_MAX;  /* inited to suppress compiler warn */
                U8 bits_differing = 0;
                Size_t full_cp_count = 0;
                bool first_time = TRUE;

                /* Go through the bytes and find the bit positions that differ
                 * */
                invlist_iterinit(cp_list);
                while (invlist_iternext(cp_list, &this_start, &this_end)) {
                    unsigned int i = this_start;

                    if (first_time) {
                        if (! UVCHR_IS_INVARIANT(i)) {
                            goto done_anyofm;
                        }

                        first_time = FALSE;
                        lowest_cp = this_start;

                        /* We have set up the code point to compare with.
                         * Don't compare it with itself */
                        i++;
                    }

                    /* Find the bit positions that differ from the lowest code
                     * point in the node.  Keep track of all such positions by
                     * OR'ing */
                    for (; i <= this_end; i++) {
                        if (! UVCHR_IS_INVARIANT(i)) {
                            goto done_anyofm;
                        }

                        bits_differing  |= i ^ lowest_cp;
                    }

                    full_cp_count += this_end - this_start + 1;
                }
                invlist_iterfinish(cp_list);

                /* At the end of the loop, we count how many bits differ from
                 * the bits in lowest code point, call the count 'd'.  If the
                 * set we found contains 2**d elements, it is the closure of
                 * all code points that differ only in those bit positions.  To
                 * convince yourself of that, first note that the number in the
                 * closure must be a power of 2, which we test for.  The only
                 * way we could have that count and it be some differing set,
                 * is if we got some code points that don't differ from the
                 * lowest code point in any position, but do differ from each
                 * other in some other position.  That means one code point has
                 * a 1 in that position, and another has a 0.  But that would
                 * mean that one of them differs from the lowest code point in
                 * that position, which possibility we've already excluded.  */
                if (  (inverted || full_cp_count > 1)
                    && full_cp_count == 1U << PL_bitcount[bits_differing])
                {
                    U8 ANYOFM_mask;

                    op = ANYOFM + inverted;;

                    /* We need to make the bits that differ be 0's */
                    ANYOFM_mask = ~ bits_differing; /* This goes into FLAGS */

                    /* The argument is the lowest code point */
                    ret = reganode(pRExC_state, op, lowest_cp);
                    FLAGS(REGNODE_p(ret)) = ANYOFM_mask;
                }
            }
          done_anyofm:

            if (inverted) {
                _invlist_invert(cp_list);
            }

            if (op != END) {
                goto not_anyof;
            }
        }

        if (! (anyof_flags & ANYOF_LOCALE_FLAGS)) {
            PERL_UINT_FAST8_T type;
            SV * intersection = NULL;
            SV* d_invlist = NULL;

            /* See if this matches any of the POSIX classes.  The POSIXA and
             * POSIXD ones are about the same speed as ANYOF ops, but take less
             * room; the ones that have above-Latin1 code point matches are
             * somewhat faster than ANYOF.  */

            for (type = POSIXA; type >= POSIXD; type--) {
                int posix_class;

                if (type == POSIXL) {   /* But not /l posix classes */
                    continue;
                }

                for (posix_class = 0;
                     posix_class <= _HIGHEST_REGCOMP_DOT_H_SYNC;
                     posix_class++)
                {
                    SV** our_code_points = &cp_list;
                    SV** official_code_points;
                    int try_inverted;

                    if (type == POSIXA) {
                        official_code_points = &PL_Posix_ptrs[posix_class];
                    }
                    else {
                        official_code_points = &PL_XPosix_ptrs[posix_class];
                    }

                    /* Skip non-existent classes of this type.  e.g. \v only
                     * has an entry in PL_XPosix_ptrs */
                    if (! *official_code_points) {
                        continue;
                    }

                    /* Try both the regular class, and its inversion */
                    for (try_inverted = 0; try_inverted < 2; try_inverted++) {
                        bool this_inverted = invert ^ try_inverted;

                        if (type != POSIXD) {

                            /* This class that isn't /d can't match if we have
                             * /d dependencies */
                            if (has_runtime_dependency
                                                    & HAS_D_RUNTIME_DEPENDENCY)
                            {
                                continue;
                            }
                        }
                        else /* is /d */ if (! this_inverted) {

                            /* /d classes don't match anything non-ASCII below
                             * 256 unconditionally (which cp_list contains) */
                            _invlist_intersection(cp_list, PL_UpperLatin1,
                                                           &intersection);
                            if (_invlist_len(intersection) != 0) {
                                continue;
                            }

                            SvREFCNT_dec(d_invlist);
                            d_invlist = invlist_clone(cp_list, NULL);

                            /* But under UTF-8 it turns into using /u rules.
                             * Add the things it matches under these conditions
                             * so that we check below that these are identical
                             * to what the tested class should match */
                            if (upper_latin1_only_utf8_matches) {
                                _invlist_union(
                                            d_invlist,
                                            upper_latin1_only_utf8_matches,
                                            &d_invlist);
                            }
                            our_code_points = &d_invlist;
                        }
                        else {  /* POSIXD, inverted.  If this doesn't have this
                                   flag set, it isn't /d. */
                            if (! (anyof_flags & ANYOF_SHARED_d_MATCHES_ALL_NON_UTF8_NON_ASCII_non_d_WARN_SUPER))
                            {
                                continue;
                            }
                            our_code_points = &cp_list;
                        }

                        /* Here, have weeded out some things.  We want to see
                         * if the list of characters this node contains
                         * ('*our_code_points') precisely matches those of the
                         * class we are currently checking against
                         * ('*official_code_points'). */
                        if (_invlistEQ(*our_code_points,
                                       *official_code_points,
                                       try_inverted))
                        {
                            /* Here, they precisely match.  Optimize this ANYOF
                             * node into its equivalent POSIX one of the
                             * correct type, possibly inverted */
                            ret = reg_node(pRExC_state, (try_inverted)
                                                        ? type + NPOSIXA
                                                                - POSIXA
                                                        : type);
                            FLAGS(REGNODE_p(ret)) = posix_class;
                            SvREFCNT_dec(d_invlist);
                            SvREFCNT_dec(intersection);
                            goto not_anyof;
                        }
                    }
                }
            }
            SvREFCNT_dec(d_invlist);
            SvREFCNT_dec(intersection);
        }

        /* If didn't find an optimization and there is no need for a
        * bitmap, optimize to indicate that */
        if (     start[0] >= NUM_ANYOF_CODE_POINTS
            && ! LOC
            && ! upper_latin1_only_utf8_matches
            &&   anyof_flags == 0)
        {
            UV highest_cp = invlist_highest(cp_list);

            /* If the lowest and highest code point in the class have the same
             * UTF-8 first byte, then all do, and we can store that byte for
             * regexec.c to use so that it can more quickly scan the target
             * string for potential matches for this class.  We co-opt the the
             * flags field for this.  Zero means, they don't have the same
             * first byte.  We do accept here very large code points (for
             * future use), but don't bother with this optimization for them,
             * as it would cause other complications */
            if (highest_cp > IV_MAX) {
                anyof_flags = 0;
            }
            else {
                U8 low_utf8[UTF8_MAXBYTES+1];
                U8 high_utf8[UTF8_MAXBYTES+1];

                (void) uvchr_to_utf8(low_utf8, start[0]);
                (void) uvchr_to_utf8(high_utf8, invlist_highest(cp_list));

                anyof_flags = (low_utf8[0] == high_utf8[0])
                            ? low_utf8[0]
                            : 0;
            }

            op = ANYOFH;
        }
    }   /* End of seeing if can optimize it into a different node */

  is_anyof: /* It's going to be an ANYOF node. */
    if (op != ANYOFH) {
        op = (has_runtime_dependency & HAS_D_RUNTIME_DEPENDENCY)
             ? ANYOFD
             : ((posixl)
                ? ANYOFPOSIXL
                : ((LOC)
                   ? ANYOFL
                   : ANYOF));
    }

    ret = regnode_guts(pRExC_state, op, regarglen[op], "anyof");
    FILL_NODE(ret, op);        /* We set the argument later */
    RExC_emit += 1 + regarglen[op];
    ANYOF_FLAGS(REGNODE_p(ret)) = anyof_flags;

    /* Here, <cp_list> contains all the code points we can determine at
     * compile time that match under all conditions.  Go through it, and
     * for things that belong in the bitmap, put them there, and delete from
     * <cp_list>.  While we are at it, see if everything above 255 is in the
     * list, and if so, set a flag to speed up execution */

    populate_ANYOF_from_invlist(REGNODE_p(ret), &cp_list);

    if (posixl) {
        ANYOF_POSIXL_SET_TO_BITMAP(REGNODE_p(ret), posixl);
    }

    if (invert) {
        ANYOF_FLAGS(REGNODE_p(ret)) |= ANYOF_INVERT;
    }

    /* Here, the bitmap has been populated with all the Latin1 code points that
     * always match.  Can now add to the overall list those that match only
     * when the target string is UTF-8 (<upper_latin1_only_utf8_matches>).
     * */
    if (upper_latin1_only_utf8_matches) {
	if (cp_list) {
	    _invlist_union(cp_list,
                           upper_latin1_only_utf8_matches,
                           &cp_list);
	    SvREFCNT_dec_NN(upper_latin1_only_utf8_matches);
	}
	else {
	    cp_list = upper_latin1_only_utf8_matches;
	}
        ANYOF_FLAGS(REGNODE_p(ret)) |= ANYOF_SHARED_d_UPPER_LATIN1_UTF8_STRING_MATCHES_non_d_RUNTIME_USER_PROP;
    }

    set_ANYOF_arg(pRExC_state, REGNODE_p(ret), cp_list,
                  (HAS_NONLOCALE_RUNTIME_PROPERTY_DEFINITION)
                   ? listsv : NULL,
                  only_utf8_locale_list);
    return ret;

  not_anyof:

    /* Here, the node is getting optimized into something that's not an ANYOF
     * one.  Finish up. */

    Set_Node_Offset_Length(REGNODE_p(ret), orig_parse - RExC_start,
                                           RExC_parse - orig_parse);;
    SvREFCNT_dec(cp_list);;
    return ret;