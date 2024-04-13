STATIC regnode_offset
S_regatom(pTHX_ RExC_state_t *pRExC_state, I32 *flagp, U32 depth)
{
    dVAR;
    regnode_offset ret = 0;
    I32 flags = 0;
    char *parse_start;
    U8 op;
    int invert = 0;
    U8 arg;

    GET_RE_DEBUG_FLAGS_DECL;

    *flagp = WORST;		/* Tentatively. */

    DEBUG_PARSE("atom");

    PERL_ARGS_ASSERT_REGATOM;

  tryagain:
    parse_start = RExC_parse;
    assert(RExC_parse < RExC_end);
    switch ((U8)*RExC_parse) {
    case '^':
	RExC_seen_zerolen++;
	nextchar(pRExC_state);
	if (RExC_flags & RXf_PMf_MULTILINE)
	    ret = reg_node(pRExC_state, MBOL);
	else
	    ret = reg_node(pRExC_state, SBOL);
        Set_Node_Length(REGNODE_p(ret), 1); /* MJD */
	break;
    case '$':
	nextchar(pRExC_state);
	if (*RExC_parse)
	    RExC_seen_zerolen++;
	if (RExC_flags & RXf_PMf_MULTILINE)
	    ret = reg_node(pRExC_state, MEOL);
	else
	    ret = reg_node(pRExC_state, SEOL);
        Set_Node_Length(REGNODE_p(ret), 1); /* MJD */
	break;
    case '.':
	nextchar(pRExC_state);
	if (RExC_flags & RXf_PMf_SINGLELINE)
	    ret = reg_node(pRExC_state, SANY);
	else
	    ret = reg_node(pRExC_state, REG_ANY);
	*flagp |= HASWIDTH|SIMPLE;
	MARK_NAUGHTY(1);
        Set_Node_Length(REGNODE_p(ret), 1); /* MJD */
	break;
    case '[':
    {
	char * const oregcomp_parse = ++RExC_parse;
        ret = regclass(pRExC_state, flagp, depth+1,
                       FALSE, /* means parse the whole char class */
                       TRUE, /* allow multi-char folds */
                       FALSE, /* don't silence non-portable warnings. */
                       (bool) RExC_strict,
                       TRUE, /* Allow an optimized regnode result */
                       NULL);
        if (ret == 0) {
            RETURN_FAIL_ON_RESTART_FLAGP(flagp);
            FAIL2("panic: regclass returned failure to regatom, flags=%#" UVxf,
                  (UV) *flagp);
        }
	if (*RExC_parse != ']') {
	    RExC_parse = oregcomp_parse;
	    vFAIL("Unmatched [");
	}
	nextchar(pRExC_state);
        Set_Node_Length(REGNODE_p(ret), RExC_parse - oregcomp_parse + 1); /* MJD */
	break;
    }
    case '(':
	nextchar(pRExC_state);
        ret = reg(pRExC_state, 2, &flags, depth+1);
	if (ret == 0) {
		if (flags & TRYAGAIN) {
		    if (RExC_parse >= RExC_end) {
			 /* Make parent create an empty node if needed. */
			*flagp |= TRYAGAIN;
			return(0);
		    }
		    goto tryagain;
		}
                RETURN_FAIL_ON_RESTART(flags, flagp);
                FAIL2("panic: reg returned failure to regatom, flags=%#" UVxf,
                                                                 (UV) flags);
	}
	*flagp |= flags&(HASWIDTH|SPSTART|SIMPLE|POSTPONED);
	break;
    case '|':
    case ')':
	if (flags & TRYAGAIN) {
	    *flagp |= TRYAGAIN;
	    return 0;
	}
	vFAIL("Internal urp");
				/* Supposed to be caught earlier. */
	break;
    case '?':
    case '+':
    case '*':
	RExC_parse++;
	vFAIL("Quantifier follows nothing");
	break;
    case '\\':
	/* Special Escapes

	   This switch handles escape sequences that resolve to some kind
	   of special regop and not to literal text. Escape sequences that
	   resolve to literal text are handled below in the switch marked
	   "Literal Escapes".

	   Every entry in this switch *must* have a corresponding entry
	   in the literal escape switch. However, the opposite is not
	   required, as the default for this switch is to jump to the
	   literal text handling code.
	*/
	RExC_parse++;
	switch ((U8)*RExC_parse) {
	/* Special Escapes */
	case 'A':
	    RExC_seen_zerolen++;
	    ret = reg_node(pRExC_state, SBOL);
            /* SBOL is shared with /^/ so we set the flags so we can tell
             * /\A/ from /^/ in split. */
            FLAGS(REGNODE_p(ret)) = 1;
	    *flagp |= SIMPLE;
	    goto finish_meta_pat;
	case 'G':
	    ret = reg_node(pRExC_state, GPOS);
            RExC_seen |= REG_GPOS_SEEN;
	    *flagp |= SIMPLE;
	    goto finish_meta_pat;
	case 'K':
	    RExC_seen_zerolen++;
	    ret = reg_node(pRExC_state, KEEPS);
	    *flagp |= SIMPLE;
	    /* XXX:dmq : disabling in-place substitution seems to
	     * be necessary here to avoid cases of memory corruption, as
	     * with: C<$_="x" x 80; s/x\K/y/> -- rgs
	     */
            RExC_seen |= REG_LOOKBEHIND_SEEN;
	    goto finish_meta_pat;
	case 'Z':
	    ret = reg_node(pRExC_state, SEOL);
	    *flagp |= SIMPLE;
	    RExC_seen_zerolen++;		/* Do not optimize RE away */
	    goto finish_meta_pat;
	case 'z':
	    ret = reg_node(pRExC_state, EOS);
	    *flagp |= SIMPLE;
	    RExC_seen_zerolen++;		/* Do not optimize RE away */
	    goto finish_meta_pat;
	case 'C':
	    vFAIL("\\C no longer supported");
	case 'X':
	    ret = reg_node(pRExC_state, CLUMP);
	    *flagp |= HASWIDTH;
	    goto finish_meta_pat;

	case 'W':
            invert = 1;
            /* FALLTHROUGH */
	case 'w':
            arg = ANYOF_WORDCHAR;
            goto join_posix;

	case 'B':
            invert = 1;
            /* FALLTHROUGH */
	case 'b':
          {
            U8 flags = 0;
	    regex_charset charset = get_regex_charset(RExC_flags);

	    RExC_seen_zerolen++;
            RExC_seen |= REG_LOOKBEHIND_SEEN;
	    op = BOUND + charset;

	    if (RExC_parse >= RExC_end || *(RExC_parse + 1) != '{') {
                flags = TRADITIONAL_BOUND;
                if (op > BOUNDA) {  /* /aa is same as /a */
                    op = BOUNDA;
                }
            }
            else {
                STRLEN length;
                char name = *RExC_parse;
                char * endbrace = NULL;
                RExC_parse += 2;
                endbrace = (char *) memchr(RExC_parse, '}', RExC_end - RExC_parse);

                if (! endbrace) {
                    vFAIL2("Missing right brace on \\%c{}", name);
                }
                /* XXX Need to decide whether to take spaces or not.  Should be
                 * consistent with \p{}, but that currently is SPACE, which
                 * means vertical too, which seems wrong
                 * while (isBLANK(*RExC_parse)) {
                    RExC_parse++;
                }*/
                if (endbrace == RExC_parse) {
                    RExC_parse++;  /* After the '}' */
                    vFAIL2("Empty \\%c{}", name);
                }
                length = endbrace - RExC_parse;
                /*while (isBLANK(*(RExC_parse + length - 1))) {
                    length--;
                }*/
                switch (*RExC_parse) {
                    case 'g':
                        if (    length != 1
                            && (memNEs(RExC_parse + 1, length - 1, "cb")))
                        {
                            goto bad_bound_type;
                        }
                        flags = GCB_BOUND;
                        break;
                    case 'l':
                        if (length != 2 || *(RExC_parse + 1) != 'b') {
                            goto bad_bound_type;
                        }
                        flags = LB_BOUND;
                        break;
                    case 's':
                        if (length != 2 || *(RExC_parse + 1) != 'b') {
                            goto bad_bound_type;
                        }
                        flags = SB_BOUND;
                        break;
                    case 'w':
                        if (length != 2 || *(RExC_parse + 1) != 'b') {
                            goto bad_bound_type;
                        }
                        flags = WB_BOUND;
                        break;
                    default:
                      bad_bound_type:
                        RExC_parse = endbrace;
			vFAIL2utf8f(
                            "'%" UTF8f "' is an unknown bound type",
			    UTF8fARG(UTF, length, endbrace - length));
                        NOT_REACHED; /*NOTREACHED*/
                }
                RExC_parse = endbrace;
                REQUIRE_UNI_RULES(flagp, 0);

                if (op == BOUND) {
                    op = BOUNDU;
                }
                else if (op >= BOUNDA) {  /* /aa is same as /a */
                    op = BOUNDU;
                    length += 4;

                    /* Don't have to worry about UTF-8, in this message because
                     * to get here the contents of the \b must be ASCII */
                    ckWARN4reg(RExC_parse + 1,  /* Include the '}' in msg */
                              "Using /u for '%.*s' instead of /%s",
                              (unsigned) length,
                              endbrace - length + 1,
                              (charset == REGEX_ASCII_RESTRICTED_CHARSET)
                              ? ASCII_RESTRICT_PAT_MODS
                              : ASCII_MORE_RESTRICT_PAT_MODS);
                }
	    }

            if (op == BOUND) {
                RExC_seen_d_op = TRUE;
            }
            else if (op == BOUNDL) {
                RExC_contains_locale = 1;
            }

            if (invert) {
                op += NBOUND - BOUND;
            }

	    ret = reg_node(pRExC_state, op);
            FLAGS(REGNODE_p(ret)) = flags;

	    *flagp |= SIMPLE;

	    goto finish_meta_pat;
          }

	case 'D':
            invert = 1;
            /* FALLTHROUGH */
	case 'd':
            arg = ANYOF_DIGIT;
            if (! DEPENDS_SEMANTICS) {
                goto join_posix;
            }

            /* \d doesn't have any matches in the upper Latin1 range, hence /d
             * is equivalent to /u.  Changing to /u saves some branches at
             * runtime */
            op = POSIXU;
            goto join_posix_op_known;

	case 'R':
	    ret = reg_node(pRExC_state, LNBREAK);
	    *flagp |= HASWIDTH|SIMPLE;
	    goto finish_meta_pat;

	case 'H':
            invert = 1;
            /* FALLTHROUGH */
	case 'h':
	    arg = ANYOF_BLANK;
            op = POSIXU;
            goto join_posix_op_known;

	case 'V':
            invert = 1;
            /* FALLTHROUGH */
	case 'v':
	    arg = ANYOF_VERTWS;
            op = POSIXU;
            goto join_posix_op_known;

	case 'S':
            invert = 1;
            /* FALLTHROUGH */
	case 's':
            arg = ANYOF_SPACE;

          join_posix:

	    op = POSIXD + get_regex_charset(RExC_flags);
            if (op > POSIXA) {  /* /aa is same as /a */
                op = POSIXA;
            }
            else if (op == POSIXL) {
                RExC_contains_locale = 1;
            }
            else if (op == POSIXD) {
                RExC_seen_d_op = TRUE;
            }

          join_posix_op_known:

            if (invert) {
                op += NPOSIXD - POSIXD;
            }

	    ret = reg_node(pRExC_state, op);
            FLAGS(REGNODE_p(ret)) = namedclass_to_classnum(arg);

	    *flagp |= HASWIDTH|SIMPLE;
            /* FALLTHROUGH */

          finish_meta_pat:
            if (   UCHARAT(RExC_parse + 1) == '{'
                && UNLIKELY(! new_regcurly(RExC_parse + 1, RExC_end)))
            {
                RExC_parse += 2;
                vFAIL("Unescaped left brace in regex is illegal here");
            }
	    nextchar(pRExC_state);
            Set_Node_Length(REGNODE_p(ret), 2); /* MJD */
	    break;
	case 'p':
	case 'P':
            RExC_parse--;

            ret = regclass(pRExC_state, flagp, depth+1,
                           TRUE, /* means just parse this element */
                           FALSE, /* don't allow multi-char folds */
                           FALSE, /* don't silence non-portable warnings.  It
                                     would be a bug if these returned
                                     non-portables */
                           (bool) RExC_strict,
                           TRUE, /* Allow an optimized regnode result */
                           NULL);
            RETURN_FAIL_ON_RESTART_FLAGP(flagp);
            /* regclass() can only return RESTART_PARSE and NEED_UTF8 if
             * multi-char folds are allowed.  */
            if (!ret)
                FAIL2("panic: regclass returned failure to regatom, flags=%#" UVxf,
                      (UV) *flagp);

            RExC_parse--;

            Set_Node_Offset(REGNODE_p(ret), parse_start);
            Set_Node_Cur_Length(REGNODE_p(ret), parse_start - 2);
            nextchar(pRExC_state);
	    break;
        case 'N':
            /* Handle \N, \N{} and \N{NAMED SEQUENCE} (the latter meaning the
             * \N{...} evaluates to a sequence of more than one code points).
             * The function call below returns a regnode, which is our result.
             * The parameters cause it to fail if the \N{} evaluates to a
             * single code point; we handle those like any other literal.  The
             * reason that the multicharacter case is handled here and not as
             * part of the EXACtish code is because of quantifiers.  In
             * /\N{BLAH}+/, the '+' applies to the whole thing, and doing it
             * this way makes that Just Happen. dmq.
             * join_exact() will join this up with adjacent EXACTish nodes
             * later on, if appropriate. */
            ++RExC_parse;
            if (grok_bslash_N(pRExC_state,
                              &ret,     /* Want a regnode returned */
                              NULL,     /* Fail if evaluates to a single code
                                           point */
                              NULL,     /* Don't need a count of how many code
                                           points */
                              flagp,
                              RExC_strict,
                              depth)
            ) {
                break;
            }

            RETURN_FAIL_ON_RESTART_FLAGP(flagp);

            /* Here, evaluates to a single code point.  Go get that */
            RExC_parse = parse_start;
            goto defchar;

	case 'k':    /* Handle \k<NAME> and \k'NAME' */
      parse_named_seq:
        {
            char ch;
            if (   RExC_parse >= RExC_end - 1
                || ((   ch = RExC_parse[1]) != '<'
                                      && ch != '\''
                                      && ch != '{'))
            {
	        RExC_parse++;
		/* diag_listed_as: Sequence \%s... not terminated in regex; marked by <-- HERE in m/%s/ */
	        vFAIL2("Sequence %.2s... not terminated", parse_start);
	    } else {
		RExC_parse += 2;
                ret = handle_named_backref(pRExC_state,
                                           flagp,
                                           parse_start,
                                           (ch == '<')
                                           ? '>'
                                           : (ch == '{')
                                             ? '}'
                                             : '\'');
            }
            break;
	}
	case 'g':
	case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	    {
		I32 num;
		bool hasbrace = 0;

		if (*RExC_parse == 'g') {
                    bool isrel = 0;

		    RExC_parse++;
		    if (*RExC_parse == '{') {
		        RExC_parse++;
		        hasbrace = 1;
		    }
		    if (*RExC_parse == '-') {
		        RExC_parse++;
		        isrel = 1;
		    }
		    if (hasbrace && !isDIGIT(*RExC_parse)) {
		        if (isrel) RExC_parse--;
                        RExC_parse -= 2;
		        goto parse_named_seq;
                    }

                    if (RExC_parse >= RExC_end) {
                        goto unterminated_g;
                    }
                    num = S_backref_value(RExC_parse, RExC_end);
                    if (num == 0)
                        vFAIL("Reference to invalid group 0");
                    else if (num == I32_MAX) {
                         if (isDIGIT(*RExC_parse))
			    vFAIL("Reference to nonexistent group");
                        else
                          unterminated_g:
                            vFAIL("Unterminated \\g... pattern");
                    }

                    if (isrel) {
                        num = RExC_npar - num;
                        if (num < 1)
                            vFAIL("Reference to nonexistent or unclosed group");
                    }
                }
                else {
                    num = S_backref_value(RExC_parse, RExC_end);
                    /* bare \NNN might be backref or octal - if it is larger
                     * than or equal RExC_npar then it is assumed to be an
                     * octal escape. Note RExC_npar is +1 from the actual
                     * number of parens. */
                    /* Note we do NOT check if num == I32_MAX here, as that is
                     * handled by the RExC_npar check */

                    if (
                        /* any numeric escape < 10 is always a backref */
                        num > 9
                        /* any numeric escape < RExC_npar is a backref */
                        && num >= RExC_npar
                        /* cannot be an octal escape if it starts with 8 */
                        && *RExC_parse != '8'
                        /* cannot be an octal escape it it starts with 9 */
                        && *RExC_parse != '9'
                    ) {
                        /* Probably not meant to be a backref, instead likely
                         * to be an octal character escape, e.g. \35 or \777.
                         * The above logic should make it obvious why using
                         * octal escapes in patterns is problematic. - Yves */
                        RExC_parse = parse_start;
                        goto defchar;
                    }
                }

                /* At this point RExC_parse points at a numeric escape like
                 * \12 or \88 or something similar, which we should NOT treat
                 * as an octal escape. It may or may not be a valid backref
                 * escape. For instance \88888888 is unlikely to be a valid
                 * backref. */
                while (isDIGIT(*RExC_parse))
                    RExC_parse++;
                if (hasbrace) {
                    if (*RExC_parse != '}')
                        vFAIL("Unterminated \\g{...} pattern");
                    RExC_parse++;
                }
                if (num >= (I32)RExC_npar) {

                    /* It might be a forward reference; we can't fail until we
                     * know, by completing the parse to get all the groups, and
                     * then reparsing */
                    if (ALL_PARENS_COUNTED)  {
                        if (num >= RExC_total_parens)  {
                            vFAIL("Reference to nonexistent group");
                        }
                    }
                    else {
                        REQUIRE_PARENS_PASS;
                    }
                }
                RExC_sawback = 1;
                ret = reganode(pRExC_state,
                               ((! FOLD)
                                 ? REF
                                 : (ASCII_FOLD_RESTRICTED)
                                   ? REFFA
                                   : (AT_LEAST_UNI_SEMANTICS)
                                     ? REFFU
                                     : (LOC)
                                       ? REFFL
                                       : REFF),
                                num);
                if (OP(REGNODE_p(ret)) == REFF) {
                    RExC_seen_d_op = TRUE;
                }
                *flagp |= HASWIDTH;

                /* override incorrect value set in reganode MJD */
                Set_Node_Offset(REGNODE_p(ret), parse_start);
                Set_Node_Cur_Length(REGNODE_p(ret), parse_start-1);
                skip_to_be_ignored_text(pRExC_state, &RExC_parse,
                                        FALSE /* Don't force to /x */ );
	    }
	    break;
	case '\0':
	    if (RExC_parse >= RExC_end)
		FAIL("Trailing \\");
	    /* FALLTHROUGH */
	default:
	    /* Do not generate "unrecognized" warnings here, we fall
	       back into the quick-grab loop below */
            RExC_parse = parse_start;
	    goto defchar;
	} /* end of switch on a \foo sequence */
	break;

    case '#':

        /* '#' comments should have been spaced over before this function was
         * called */
        assert((RExC_flags & RXf_PMf_EXTENDED) == 0);
	/*
        if (RExC_flags & RXf_PMf_EXTENDED) {
	    RExC_parse = reg_skipcomment( pRExC_state, RExC_parse );
	    if (RExC_parse < RExC_end)
		goto tryagain;
	}
        */

	/* FALLTHROUGH */

    default:
	  defchar: {

            /* Here, we have determined that the next thing is probably a
             * literal character.  RExC_parse points to the first byte of its
             * definition.  (It still may be an escape sequence that evaluates
             * to a single character) */

	    STRLEN len = 0;
	    UV ender = 0;
	    char *p;
	    char *s;

/* This allows us to fill a node with just enough spare so that if the final
 * character folds, its expansion is guaranteed to fit */
#define MAX_NODE_STRING_SIZE (255-UTF8_MAXBYTES_CASE)

	    char *s0;
	    U8 upper_parse = MAX_NODE_STRING_SIZE;

            /* We start out as an EXACT node, even if under /i, until we find a
             * character which is in a fold.  The algorithm now segregates into
             * separate nodes, characters that fold from those that don't under
             * /i.  (This hopefully will create nodes that are fixed strings
             * even under /i, giving the optimizer something to grab on to.)
             * So, if a node has something in it and the next character is in
             * the opposite category, that node is closed up, and the function
             * returns.  Then regatom is called again, and a new node is
             * created for the new category. */
            U8 node_type = EXACT;

            /* Assume the node will be fully used; the excess is given back at
             * the end.  We can't make any other length assumptions, as a byte
             * input sequence could shrink down. */
            Ptrdiff_t initial_size = STR_SZ(256);

            bool next_is_quantifier;
            char * oldp = NULL;

            /* We can convert EXACTF nodes to EXACTFU if they contain only
             * characters that match identically regardless of the target
             * string's UTF8ness.  The reason to do this is that EXACTF is not
             * trie-able, EXACTFU is, and EXACTFU requires fewer operations at
             * runtime.
             *
             * Similarly, we can convert EXACTFL nodes to EXACTFLU8 if they
             * contain only above-Latin1 characters (hence must be in UTF8),
             * which don't participate in folds with Latin1-range characters,
             * as the latter's folds aren't known until runtime. */
            bool maybe_exactfu = FOLD && (DEPENDS_SEMANTICS || LOC);

            /* Single-character EXACTish nodes are almost always SIMPLE.  This
             * allows us to override this as encountered */
            U8 maybe_SIMPLE = SIMPLE;

            /* Does this node contain something that can't match unless the
             * target string is (also) in UTF-8 */
            bool requires_utf8_target = FALSE;

            /* The sequence 'ss' is problematic in non-UTF-8 patterns. */
            bool has_ss = FALSE;

            /* So is the MICRO SIGN */
            bool has_micro_sign = FALSE;

            /* Allocate an EXACT node.  The node_type may change below to
             * another EXACTish node, but since the size of the node doesn't
             * change, it works */
            ret = regnode_guts(pRExC_state, node_type, initial_size, "exact");
            FILL_NODE(ret, node_type);
            RExC_emit++;

	    s = STRING(REGNODE_p(ret));

            s0 = s;

	  reparse:

            /* This breaks under rare circumstances.  If folding, we do not
             * want to split a node at a character that is a non-final in a
             * multi-char fold, as an input string could just happen to want to
             * match across the node boundary.  The code at the end of the loop
             * looks for this, and backs off until it finds not such a
             * character, but it is possible (though extremely, extremely
             * unlikely) for all characters in the node to be non-final fold
             * ones, in which case we just leave the node fully filled, and
             * hope that it doesn't match the string in just the wrong place */

            assert( ! UTF     /* Is at the beginning of a character */
                   || UTF8_IS_INVARIANT(UCHARAT(RExC_parse))
                   || UTF8_IS_START(UCHARAT(RExC_parse)));

            /* Here, we have a literal character.  Find the maximal string of
             * them in the input that we can fit into a single EXACTish node.
             * We quit at the first non-literal or when the node gets full, or
             * under /i the categorization of folding/non-folding character
             * changes */
	    for (p = RExC_parse; len < upper_parse && p < RExC_end; ) {

                /* In most cases each iteration adds one byte to the output.
                 * The exceptions override this */
                Size_t added_len = 1;

		oldp = p;

                /* White space has already been ignored */
                assert(   (RExC_flags & RXf_PMf_EXTENDED) == 0
                       || ! is_PATWS_safe((p), RExC_end, UTF));

		switch ((U8)*p) {
		case '^':
		case '$':
		case '.':
		case '[':
		case '(':
		case ')':
		case '|':
		    goto loopdone;
		case '\\':
		    /* Literal Escapes Switch

		       This switch is meant to handle escape sequences that
		       resolve to a literal character.

		       Every escape sequence that represents something
		       else, like an assertion or a char class, is handled
		       in the switch marked 'Special Escapes' above in this
		       routine, but also has an entry here as anything that
		       isn't explicitly mentioned here will be treated as
		       an unescaped equivalent literal.
		    */

		    switch ((U8)*++p) {

		    /* These are all the special escapes. */
		    case 'A':             /* Start assertion */
		    case 'b': case 'B':   /* Word-boundary assertion*/
		    case 'C':             /* Single char !DANGEROUS! */
		    case 'd': case 'D':   /* digit class */
		    case 'g': case 'G':   /* generic-backref, pos assertion */
		    case 'h': case 'H':   /* HORIZWS */
		    case 'k': case 'K':   /* named backref, keep marker */
		    case 'p': case 'P':   /* Unicode property */
		              case 'R':   /* LNBREAK */
		    case 's': case 'S':   /* space class */
		    case 'v': case 'V':   /* VERTWS */
		    case 'w': case 'W':   /* word class */
                    case 'X':             /* eXtended Unicode "combining
                                             character sequence" */
		    case 'z': case 'Z':   /* End of line/string assertion */
			--p;
			goto loopdone;

	            /* Anything after here is an escape that resolves to a
	               literal. (Except digits, which may or may not)
	             */
		    case 'n':
			ender = '\n';
			p++;
			break;
		    case 'N': /* Handle a single-code point named character. */
                        RExC_parse = p + 1;
                        if (! grok_bslash_N(pRExC_state,
                                            NULL,   /* Fail if evaluates to
                                                       anything other than a
                                                       single code point */
                                            &ender, /* The returned single code
                                                       point */
                                            NULL,   /* Don't need a count of
                                                       how many code points */
                                            flagp,
                                            RExC_strict,
                                            depth)
                        ) {
                            if (*flagp & NEED_UTF8)
                                FAIL("panic: grok_bslash_N set NEED_UTF8");
                            RETURN_FAIL_ON_RESTART_FLAGP(flagp);

                            /* Here, it wasn't a single code point.  Go close
                             * up this EXACTish node.  The switch() prior to
                             * this switch handles the other cases */
                            RExC_parse = p = oldp;
                            goto loopdone;
                        }
                        p = RExC_parse;
                        RExC_parse = parse_start;

                        /* The \N{} means the pattern, if previously /d,
                         * becomes /u.  That means it can't be an EXACTF node,
                         * but an EXACTFU */
                        if (node_type == EXACTF) {
                            node_type = EXACTFU;

                            /* If the node already contains something that
                             * differs between EXACTF and EXACTFU, reparse it
                             * as EXACTFU */
                            if (! maybe_exactfu) {
                                len = 0;
                                s = s0;
                                goto reparse;
                            }
                        }

                        break;
		    case 'r':
			ender = '\r';
			p++;
			break;
		    case 't':
			ender = '\t';
			p++;
			break;
		    case 'f':
			ender = '\f';
			p++;
			break;
		    case 'e':
			ender = ESC_NATIVE;
			p++;
			break;
		    case 'a':
			ender = '\a';
			p++;
			break;
		    case 'o':
			{
			    UV result;
			    const char* error_msg;

			    bool valid = grok_bslash_o(&p,
                                                       RExC_end,
						       &result,
						       &error_msg,
						       TO_OUTPUT_WARNINGS(p),
                                                       (bool) RExC_strict,
                                                       TRUE, /* Output warnings
                                                                for non-
                                                                portables */
                                                       UTF);
			    if (! valid) {
				RExC_parse = p;	/* going to die anyway; point
						   to exact spot of failure */
				vFAIL(error_msg);
			    }
                            UPDATE_WARNINGS_LOC(p - 1);
                            ender = result;
			    break;
			}
		    case 'x':
			{
                            UV result = UV_MAX; /* initialize to erroneous
                                                   value */
			    const char* error_msg;

			    bool valid = grok_bslash_x(&p,
                                                       RExC_end,
						       &result,
						       &error_msg,
                                                       TO_OUTPUT_WARNINGS(p),
                                                       (bool) RExC_strict,
                                                       TRUE, /* Silence warnings
                                                                for non-
                                                                portables */
                                                       UTF);
			    if (! valid) {
				RExC_parse = p;	/* going to die anyway; point
						   to exact spot of failure */
				vFAIL(error_msg);
			    }
                            UPDATE_WARNINGS_LOC(p - 1);
                            ender = result;

                            if (ender < 0x100) {
#ifdef EBCDIC
                                if (RExC_recode_x_to_native) {
                                    ender = LATIN1_TO_NATIVE(ender);
                                }
#endif
			    }
			    break;
			}
		    case 'c':
			p++;
			ender = grok_bslash_c(*p, TO_OUTPUT_WARNINGS(p));
                        UPDATE_WARNINGS_LOC(p);
                        p++;
			break;
                    case '8': case '9': /* must be a backreference */
                        --p;
                        /* we have an escape like \8 which cannot be an octal escape
                         * so we exit the loop, and let the outer loop handle this
                         * escape which may or may not be a legitimate backref. */
                        goto loopdone;
                    case '1': case '2': case '3':case '4':
		    case '5': case '6': case '7':
                        /* When we parse backslash escapes there is ambiguity
                         * between backreferences and octal escapes. Any escape
                         * from \1 - \9 is a backreference, any multi-digit
                         * escape which does not start with 0 and which when
                         * evaluated as decimal could refer to an already
                         * parsed capture buffer is a back reference. Anything
                         * else is octal.
                         *
                         * Note this implies that \118 could be interpreted as
                         * 118 OR as "\11" . "8" depending on whether there
                         * were 118 capture buffers defined already in the
                         * pattern.  */

                        /* NOTE, RExC_npar is 1 more than the actual number of
                         * parens we have seen so far, hence the "<" as opposed
                         * to "<=" */
                        if ( !isDIGIT(p[1]) || S_backref_value(p, RExC_end) < RExC_npar)
                        {  /* Not to be treated as an octal constant, go
                                   find backref */
                            --p;
                            goto loopdone;
                        }
                        /* FALLTHROUGH */
                    case '0':
			{
			    I32 flags = PERL_SCAN_SILENT_ILLDIGIT;
			    STRLEN numlen = 3;
			    ender = grok_oct(p, &numlen, &flags, NULL);
			    p += numlen;
                            if (   isDIGIT(*p)  /* like \08, \178 */
                                && ckWARN(WARN_REGEXP)
                                && numlen < 3)
                            {
				reg_warn_non_literal_string(
                                         p + 1,
                                         form_short_octal_warning(p, numlen));
                            }
			}
			break;
		    case '\0':
			if (p >= RExC_end)
			    FAIL("Trailing \\");
			/* FALLTHROUGH */
		    default:
			if (isALPHANUMERIC(*p)) {
                            /* An alpha followed by '{' is going to fail next
                             * iteration, so don't output this warning in that
                             * case */
                            if (! isALPHA(*p) || *(p + 1) != '{') {
                                ckWARN2reg(p + 1, "Unrecognized escape \\%.1s"
                                                  " passed through", p);
                            }
			}
			goto normal_default;
		    } /* End of switch on '\' */
		    break;
		case '{':
                    /* Trying to gain new uses for '{' without breaking too
                     * much existing code is hard.  The solution currently
                     * adopted is:
                     *  1)  If there is no ambiguity that a '{' should always
                     *      be taken literally, at the start of a construct, we
                     *      just do so.
                     *  2)  If the literal '{' conflicts with our desired use
                     *      of it as a metacharacter, we die.  The deprecation
                     *      cycles for this have come and gone.
                     *  3)  If there is ambiguity, we raise a simple warning.
                     *      This could happen, for example, if the user
                     *      intended it to introduce a quantifier, but slightly
                     *      misspelled the quantifier.  Without this warning,
                     *      the quantifier would silently be taken as a literal
                     *      string of characters instead of a meta construct */
		    if (len || (p > RExC_start && isALPHA_A(*(p - 1)))) {
                        if (      RExC_strict
                            || (  p > parse_start + 1
                                && isALPHA_A(*(p - 1))
                                && *(p - 2) == '\\')
                            || new_regcurly(p, RExC_end))
                        {
                            RExC_parse = p + 1;
                            vFAIL("Unescaped left brace in regex is "
                                  "illegal here");
                        }
                        ckWARNreg(p + 1, "Unescaped left brace in regex is"
                                         " passed through");
		    }
		    goto normal_default;
                case '}':
                case ']':
                    if (p > RExC_parse && RExC_strict) {
                        ckWARN2reg(p + 1, "Unescaped literal '%c'", *p);
                    }
		    /*FALLTHROUGH*/
		default:    /* A literal character */
		  normal_default:
		    if (! UTF8_IS_INVARIANT(*p) && UTF) {
			STRLEN numlen;
			ender = utf8n_to_uvchr((U8*)p, RExC_end - p,
					       &numlen, UTF8_ALLOW_DEFAULT);
			p += numlen;
		    }
		    else
			ender = (U8) *p++;
		    break;
		} /* End of switch on the literal */

		/* Here, have looked at the literal character, and <ender>
                 * contains its ordinal; <p> points to the character after it.
                 * */

                if (ender > 255) {
                    REQUIRE_UTF8(flagp);
                }

                /* We need to check if the next non-ignored thing is a
                 * quantifier.  Move <p> to after anything that should be
                 * ignored, which, as a side effect, positions <p> for the next
                 * loop iteration */
                skip_to_be_ignored_text(pRExC_state, &p,
                                        FALSE /* Don't force to /x */ );

                /* If the next thing is a quantifier, it applies to this
                 * character only, which means that this character has to be in
                 * its own node and can't just be appended to the string in an
                 * existing node, so if there are already other characters in
                 * the node, close the node with just them, and set up to do
                 * this character again next time through, when it will be the
                 * only thing in its new node */

                next_is_quantifier =    LIKELY(p < RExC_end)
                                     && UNLIKELY(ISMULT2(p));

                if (next_is_quantifier && LIKELY(len)) {
                    p = oldp;
                    goto loopdone;
                }

                /* Ready to add 'ender' to the node */

                if (! FOLD) {  /* The simple case, just append the literal */

                      not_fold_common:
                        if (UVCHR_IS_INVARIANT(ender) || ! UTF) {
                            *(s++) = (char) ender;
                        }
                        else {
                            U8 * new_s = uvchr_to_utf8((U8*)s, ender);
                            added_len = (char *) new_s - s;
                            s = (char *) new_s;

                            if (ender > 255)  {
                                requires_utf8_target = TRUE;
                            }
                        }
                }
                else if (LOC && is_PROBLEMATIC_LOCALE_FOLD_cp(ender)) {

                    /* Here are folding under /l, and the code point is
                     * problematic.  If this is the first character in the
                     * node, change the node type to folding.   Otherwise, if
                     * this is the first problematic character, close up the
                     * existing node, so can start a new node with this one */
                    if (! len) {
                        node_type = EXACTFL;
                        RExC_contains_locale = 1;
                    }
                    else if (node_type == EXACT) {
                        p = oldp;
                        goto loopdone;
                    }

                    /* This problematic code point means we can't simplify
                     * things */
                    maybe_exactfu = FALSE;

                    /* Here, we are adding a problematic fold character.
                     * "Problematic" in this context means that its fold isn't
                     * known until runtime.  (The non-problematic code points
                     * are the above-Latin1 ones that fold to also all
                     * above-Latin1.  Their folds don't vary no matter what the
                     * locale is.) But here we have characters whose fold
                     * depends on the locale.  We just add in the unfolded
                     * character, and wait until runtime to fold it */
                    goto not_fold_common;
                }
                else /* regular fold; see if actually is in a fold */
                     if (   (ender < 256 && ! IS_IN_SOME_FOLD_L1(ender))
                         || (ender > 255
                            && ! _invlist_contains_cp(PL_in_some_fold, ender)))
                {
                    /* Here, folding, but the character isn't in a fold.
                     *
                     * Start a new node if previous characters in the node were
                     * folded */
                    if (len && node_type != EXACT) {
                        p = oldp;
                        goto loopdone;
                    }

                    /* Here, continuing a node with non-folded characters.  Add
                     * this one */
                    goto not_fold_common;
                }
                else {  /* Here, does participate in some fold */

                    /* If this is the first character in the node, change its
                     * type to folding.  Otherwise, if this is the first
                     * folding character in the node, close up the existing
                     * node, so can start a new node with this one.  */
                    if (! len) {
                        node_type = compute_EXACTish(pRExC_state);
                    }
                    else if (node_type == EXACT) {
                        p = oldp;
                        goto loopdone;
                    }

                    if (UTF) {  /* Use the folded value */
                        if (UVCHR_IS_INVARIANT(ender)) {
                            *(s)++ = (U8) toFOLD(ender);
                        }
                        else {
                            ender = _to_uni_fold_flags(
                                    ender,
                                    (U8 *) s,
                                    &added_len,
                                    FOLD_FLAGS_FULL | ((ASCII_FOLD_RESTRICTED)
                                                    ? FOLD_FLAGS_NOMIX_ASCII
                                                    : 0));
                            s += added_len;

                            if (   ender > 255
                                && LIKELY(ender != GREEK_SMALL_LETTER_MU))
                            {
                                /* U+B5 folds to the MU, so its possible for a
                                 * non-UTF-8 target to match it */
                                requires_utf8_target = TRUE;
                            }
                        }
                    }
                    else {

                        /* Here is non-UTF8.  First, see if the character's
                         * fold differs between /d and /u. */
                        if (PL_fold[ender] != PL_fold_latin1[ender]) {
                            maybe_exactfu = FALSE;
                        }

#if    UNICODE_MAJOR_VERSION > 3 /* no multifolds in early Unicode */   \
   || (UNICODE_MAJOR_VERSION == 3 && (   UNICODE_DOT_VERSION > 0)       \
                                      || UNICODE_DOT_DOT_VERSION > 0)

                        /* On non-ancient Unicode versions, this includes the
                         * multi-char fold SHARP S to 'ss' */

                        if (   UNLIKELY(ender == LATIN_SMALL_LETTER_SHARP_S)
                                 || (   isALPHA_FOLD_EQ(ender, 's')
                                     && len > 0
                                     && isALPHA_FOLD_EQ(*(s-1), 's')))
                        {
                            /* Here, we have one of the following:
                             *  a)  a SHARP S.  This folds to 'ss' only under
                             *      /u rules.  If we are in that situation,
                             *      fold the SHARP S to 'ss'.  See the comments
                             *      for join_exact() as to why we fold this
                             *      non-UTF at compile time, and no others.
                             *  b)  'ss'.  When under /u, there's nothing
                             *      special needed to be done here.  The
                             *      previous iteration handled the first 's',
                             *      and this iteration will handle the second.
                             *      If, on the otherhand it's not /u, we have
                             *      to exclude the possibility of moving to /u,
                             *      so that we won't generate an unwanted
                             *      match, unless, at runtime, the target
                             *      string is in UTF-8.
                             * */

                            has_ss = TRUE;
                            maybe_exactfu = FALSE;  /* Can't generate an
                                                       EXACTFU node (unless we
                                                       already are in one) */
                            if (UNLIKELY(ender == LATIN_SMALL_LETTER_SHARP_S)) {
                                maybe_SIMPLE = 0;
                                if (node_type == EXACTFU) {
                                    *(s++) = 's';

                                    /* Let the code below add in the extra 's' */
                                    ender = 's';
                                    added_len = 2;
                                }
                            }
                        }
#endif

                        else if (UNLIKELY(ender == MICRO_SIGN)) {
                            has_micro_sign = TRUE;
                        }

                        *(s++) = (DEPENDS_SEMANTICS)
                                 ? (char) toFOLD(ender)

                                   /* Under /u, the fold of any character in
                                    * the 0-255 range happens to be its
                                    * lowercase equivalent, except for LATIN
                                    * SMALL LETTER SHARP S, which was handled
                                    * above, and the MICRO SIGN, whose fold
                                    * requires UTF-8 to represent.  */
                                 : (char) toLOWER_L1(ender);
                    }
		} /* End of adding current character to the node */

                len += added_len;

		if (next_is_quantifier) {

                    /* Here, the next input is a quantifier, and to get here,
                     * the current character is the only one in the node. */
                    goto loopdone;
		}

	    } /* End of loop through literal characters */

            /* Here we have either exhausted the input or ran out of room in
             * the node.  (If we encountered a character that can't be in the
             * node, transfer is made directly to <loopdone>, and so we
             * wouldn't have fallen off the end of the loop.)  In the latter
             * case, we artificially have to split the node into two, because
             * we just don't have enough space to hold everything.  This
             * creates a problem if the final character participates in a
             * multi-character fold in the non-final position, as a match that
             * should have occurred won't, due to the way nodes are matched,
             * and our artificial boundary.  So back off until we find a non-
             * problematic character -- one that isn't at the beginning or
             * middle of such a fold.  (Either it doesn't participate in any
             * folds, or appears only in the final position of all the folds it
             * does participate in.)  A better solution with far fewer false
             * positives, and that would fill the nodes more completely, would
             * be to actually have available all the multi-character folds to
             * test against, and to back-off only far enough to be sure that
             * this node isn't ending with a partial one.  <upper_parse> is set
             * further below (if we need to reparse the node) to include just
             * up through that final non-problematic character that this code
             * identifies, so when it is set to less than the full node, we can
             * skip the rest of this */
            if (FOLD && p < RExC_end && upper_parse == MAX_NODE_STRING_SIZE) {
                PERL_UINT_FAST8_T backup_count = 0;

                const STRLEN full_len = len;

		assert(len >= MAX_NODE_STRING_SIZE);

                /* Here, <s> points to just beyond where we have output the
                 * final character of the node.  Look backwards through the
                 * string until find a non- problematic character */

		if (! UTF) {

                    /* This has no multi-char folds to non-UTF characters */
                    if (ASCII_FOLD_RESTRICTED) {
                        goto loopdone;
                    }

                    while (--s >= s0 && IS_NON_FINAL_FOLD(*s)) {
                        backup_count++;
                    }
                    len = s - s0 + 1;
		}
                else {

                    /* Point to the first byte of the final character */
                    s = (char *) utf8_hop_back((U8 *) s, -1, (U8 *) s0);

                    while (s >= s0) {   /* Search backwards until find
                                           a non-problematic char */
                        if (UTF8_IS_INVARIANT(*s)) {

                            /* There are no ascii characters that participate
                             * in multi-char folds under /aa.  In EBCDIC, the
                             * non-ascii invariants are all control characters,
                             * so don't ever participate in any folds. */
                            if (ASCII_FOLD_RESTRICTED
                                || ! IS_NON_FINAL_FOLD(*s))
                            {
                                break;
                            }
                        }
                        else if (UTF8_IS_DOWNGRADEABLE_START(*s)) {
                            if (! IS_NON_FINAL_FOLD(EIGHT_BIT_UTF8_TO_NATIVE(
                                                                  *s, *(s+1))))
                            {
                                break;
                            }
                        }
                        else if (! _invlist_contains_cp(
                                        PL_NonFinalFold,
                                        valid_utf8_to_uvchr((U8 *) s, NULL)))
                        {
                            break;
                        }

                        /* Here, the current character is problematic in that
                         * it does occur in the non-final position of some
                         * fold, so try the character before it, but have to
                         * special case the very first byte in the string, so
                         * we don't read outside the string */
                        s = (s == s0) ? s -1 : (char *) utf8_hop((U8 *) s, -1);
                        backup_count++;
                    } /* End of loop backwards through the string */

                    /* If there were only problematic characters in the string,
                     * <s> will point to before s0, in which case the length
                     * should be 0, otherwise include the length of the
                     * non-problematic character just found */
                    len = (s < s0) ? 0 : s - s0 + UTF8SKIP(s);
		}

                /* Here, have found the final character, if any, that is
                 * non-problematic as far as ending the node without splitting
                 * it across a potential multi-char fold.  <len> contains the
                 * number of bytes in the node up-to and including that
                 * character, or is 0 if there is no such character, meaning
                 * the whole node contains only problematic characters.  In
                 * this case, give up and just take the node as-is.  We can't
                 * do any better */
                if (len == 0) {
                    len = full_len;

                } else {

                    /* Here, the node does contain some characters that aren't
                     * problematic.  If we didn't have to backup any, then the
                     * final character in the node is non-problematic, and we
                     * can take the node as-is */
                    if (backup_count == 0) {
                        goto loopdone;
                    }
                    else if (backup_count == 1) {

                        /* If the final character is problematic, but the
                         * penultimate is not, back-off that last character to
                         * later start a new node with it */
                        p = oldp;
                        goto loopdone;
                    }

                    /* Here, the final non-problematic character is earlier
                     * in the input than the penultimate character.  What we do
                     * is reparse from the beginning, going up only as far as
                     * this final ok one, thus guaranteeing that the node ends
                     * in an acceptable character.  The reason we reparse is
                     * that we know how far in the character is, but we don't
                     * know how to correlate its position with the input parse.
                     * An alternate implementation would be to build that
                     * correlation as we go along during the original parse,
                     * but that would entail extra work for every node, whereas
                     * this code gets executed only when the string is too
                     * large for the node, and the final two characters are
                     * problematic, an infrequent occurrence.  Yet another
                     * possible strategy would be to save the tail of the
                     * string, and the next time regatom is called, initialize
                     * with that.  The problem with this is that unless you
                     * back off one more character, you won't be guaranteed
                     * regatom will get called again, unless regbranch,
                     * regpiece ... are also changed.  If you do back off that
                     * extra character, so that there is input guaranteed to
                     * force calling regatom, you can't handle the case where
                     * just the first character in the node is acceptable.  I
                     * (khw) decided to try this method which doesn't have that
                     * pitfall; if performance issues are found, we can do a
                     * combination of the current approach plus that one */
                    upper_parse = len;
                    len = 0;
                    s = s0;
                    goto reparse;
                }
	    }   /* End of verifying node ends with an appropriate char */

          loopdone:   /* Jumped to when encounters something that shouldn't be
                         in the node */

            /* Free up any over-allocated space; cast is to silence bogus
             * warning in MS VC */
            change_engine_size(pRExC_state,
                                - (Ptrdiff_t) (initial_size - STR_SZ(len)));

            /* I (khw) don't know if you can get here with zero length, but the
             * old code handled this situation by creating a zero-length EXACT
             * node.  Might as well be NOTHING instead */
            if (len == 0) {
                OP(REGNODE_p(ret)) = NOTHING;
            }
            else {

                /* If the node type is EXACT here, check to see if it
                 * should be EXACTL, or EXACT_ONLY8. */
                if (node_type == EXACT) {
                    if (LOC) {
                        node_type = EXACTL;
                    }
                    else if (requires_utf8_target) {
                        node_type = EXACT_ONLY8;
                    }
                } else if (FOLD) {
                    if (    UNLIKELY(has_micro_sign || has_ss)
                        && (node_type == EXACTFU || (   node_type == EXACTF
                                                     && maybe_exactfu)))
                    {   /* These two conditions are problematic in non-UTF-8
                           EXACTFU nodes. */
                        assert(! UTF);
                        node_type = EXACTFUP;
                    }
                    else if (node_type == EXACTFL) {

                        /* 'maybe_exactfu' is deliberately set above to
                         * indicate this node type, where all code points in it
                         * are above 255 */
                        if (maybe_exactfu) {
                            node_type = EXACTFLU8;
                        }
                        else if (UNLIKELY(
                             _invlist_contains_cp(PL_HasMultiCharFold, ender)))
                        {
                            /* A character that folds to more than one will
                             * match multiple characters, so can't be SIMPLE.
                             * We don't have to worry about this with EXACTFLU8
                             * nodes just above, as they have already been
                             * folded (since the fold doesn't vary at run
                             * time).  Here, if the final character in the node
                             * folds to multiple, it can't be simple.  (This
                             * only has an effect if the node has only a single
                             * character, hence the final one, as elsewhere we
                             * turn off simple for nodes whose length > 1 */
                            maybe_SIMPLE = 0;
                        }
                    }
                    else if (node_type == EXACTF) {  /* Means is /di */

                        /* If 'maybe_exactfu' is clear, then we need to stay
                         * /di.  If it is set, it means there are no code
                         * points that match differently depending on UTF8ness
                         * of the target string, so it can become an EXACTFU
                         * node */
                        if (! maybe_exactfu) {
                            RExC_seen_d_op = TRUE;
                        }
                        else if (   isALPHA_FOLD_EQ(* STRING(REGNODE_p(ret)), 's')
                                 || isALPHA_FOLD_EQ(ender, 's'))
                        {
                            /* But, if the node begins or ends in an 's' we
                             * have to defer changing it into an EXACTFU, as
                             * the node could later get joined with another one
                             * that ends or begins with 's' creating an 'ss'
                             * sequence which would then wrongly match the
                             * sharp s without the target being UTF-8.  We
                             * create a special node that we resolve later when
                             * we join nodes together */

                            node_type = EXACTFU_S_EDGE;
                        }
                        else {
                            node_type = EXACTFU;
                        }
                    }

                    if (requires_utf8_target && node_type == EXACTFU) {
                        node_type = EXACTFU_ONLY8;
                    }
                }

                OP(REGNODE_p(ret)) = node_type;
                STR_LEN(REGNODE_p(ret)) = len;
                RExC_emit += STR_SZ(len);

                /* If the node isn't a single character, it can't be SIMPLE */
                if (len > (Size_t) ((UTF) ? UVCHR_SKIP(ender) : 1)) {
                    maybe_SIMPLE = 0;
                }

                *flagp |= HASWIDTH | maybe_SIMPLE;
            }

            Set_Node_Length(REGNODE_p(ret), p - parse_start - 1);
            RExC_parse = p;

	    {
		/* len is STRLEN which is unsigned, need to copy to signed */
		IV iv = len;
		if (iv < 0)
		    vFAIL("Internal disaster");
	    }

	} /* End of label 'defchar:' */
	break;
    } /* End of giant switch on input character */

    /* Position parse to next real character */
    skip_to_be_ignored_text(pRExC_state, &RExC_parse,
                                            FALSE /* Don't force to /x */ );
    if (   *RExC_parse == '{'
        && OP(REGNODE_p(ret)) != SBOL && ! regcurly(RExC_parse))
    {
        if (RExC_strict || new_regcurly(RExC_parse, RExC_end)) {
            RExC_parse++;
            vFAIL("Unescaped left brace in regex is illegal here");
        }
        ckWARNreg(RExC_parse + 1, "Unescaped left brace in regex is"
                                  " passed through");
    }

    return(ret);