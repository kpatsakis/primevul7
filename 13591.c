nfa_regatom(void)
{
    int		c;
    int		charclass;
    int		equiclass;
    int		collclass;
    int		got_coll_char;
    char_u	*p;
    char_u	*endp;
    char_u	*old_regparse = regparse;
    int		extra = 0;
    int		emit_range;
    int		negated;
    int		result;
    int		startc = -1;
    int		save_prev_at_start = prev_at_start;

    c = getchr();
    switch (c)
    {
	case NUL:
	    EMSG_RET_FAIL(_(e_nul_found));

	case Magic('^'):
	    EMIT(NFA_BOL);
	    break;

	case Magic('$'):
	    EMIT(NFA_EOL);
#if defined(FEAT_SYN_HL) || defined(PROTO)
	    had_eol = TRUE;
#endif
	    break;

	case Magic('<'):
	    EMIT(NFA_BOW);
	    break;

	case Magic('>'):
	    EMIT(NFA_EOW);
	    break;

	case Magic('_'):
	    c = no_Magic(getchr());
	    if (c == NUL)
		EMSG_RET_FAIL(_(e_nul_found));

	    if (c == '^')	// "\_^" is start-of-line
	    {
		EMIT(NFA_BOL);
		break;
	    }
	    if (c == '$')	// "\_$" is end-of-line
	    {
		EMIT(NFA_EOL);
#if defined(FEAT_SYN_HL) || defined(PROTO)
		had_eol = TRUE;
#endif
		break;
	    }

	    extra = NFA_ADD_NL;

	    // "\_[" is collection plus newline
	    if (c == '[')
		goto collection;

	// "\_x" is character class plus newline
	// FALLTHROUGH

	/*
	 * Character classes.
	 */
	case Magic('.'):
	case Magic('i'):
	case Magic('I'):
	case Magic('k'):
	case Magic('K'):
	case Magic('f'):
	case Magic('F'):
	case Magic('p'):
	case Magic('P'):
	case Magic('s'):
	case Magic('S'):
	case Magic('d'):
	case Magic('D'):
	case Magic('x'):
	case Magic('X'):
	case Magic('o'):
	case Magic('O'):
	case Magic('w'):
	case Magic('W'):
	case Magic('h'):
	case Magic('H'):
	case Magic('a'):
	case Magic('A'):
	case Magic('l'):
	case Magic('L'):
	case Magic('u'):
	case Magic('U'):
	    p = vim_strchr(classchars, no_Magic(c));
	    if (p == NULL)
	    {
		if (extra == NFA_ADD_NL)
		{
		    semsg(_(e_ill_char_class), c);
		    rc_did_emsg = TRUE;
		    return FAIL;
		}
		siemsg("INTERNAL: Unknown character class char: %d", c);
		return FAIL;
	    }

	    // When '.' is followed by a composing char ignore the dot, so that
	    // the composing char is matched here.
	    if (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	    {
		old_regparse = regparse;
		c = getchr();
		goto nfa_do_multibyte;
	    }
	    EMIT(nfa_classcodes[p - classchars]);
	    if (extra == NFA_ADD_NL)
	    {
		EMIT(NFA_NEWL);
		EMIT(NFA_OR);
		regflags |= RF_HASNL;
	    }
	    break;

	case Magic('n'):
	    if (reg_string)
		// In a string "\n" matches a newline character.
		EMIT(NL);
	    else
	    {
		// In buffer text "\n" matches the end of a line.
		EMIT(NFA_NEWL);
		regflags |= RF_HASNL;
	    }
	    break;

	case Magic('('):
	    if (nfa_reg(REG_PAREN) == FAIL)
		return FAIL;	    // cascaded error
	    break;

	case Magic('|'):
	case Magic('&'):
	case Magic(')'):
	    semsg(_(e_misplaced), no_Magic(c));
	    return FAIL;

	case Magic('='):
	case Magic('?'):
	case Magic('+'):
	case Magic('@'):
	case Magic('*'):
	case Magic('{'):
	    // these should follow an atom, not form an atom
	    semsg(_(e_misplaced), no_Magic(c));
	    return FAIL;

	case Magic('~'):
	    {
		char_u	    *lp;

		// Previous substitute pattern.
		// Generated as "\%(pattern\)".
		if (reg_prev_sub == NULL)
		{
		    emsg(_(e_no_previous_substitute_regular_expression));
		    return FAIL;
		}
		for (lp = reg_prev_sub; *lp != NUL; MB_CPTR_ADV(lp))
		{
		    EMIT(PTR2CHAR(lp));
		    if (lp != reg_prev_sub)
			EMIT(NFA_CONCAT);
		}
		EMIT(NFA_NOPEN);
		break;
	    }

	case Magic('1'):
	case Magic('2'):
	case Magic('3'):
	case Magic('4'):
	case Magic('5'):
	case Magic('6'):
	case Magic('7'):
	case Magic('8'):
	case Magic('9'):
	    {
		int refnum = no_Magic(c) - '1';

		if (!seen_endbrace(refnum + 1))
		    return FAIL;
		EMIT(NFA_BACKREF1 + refnum);
		rex.nfa_has_backref = TRUE;
	    }
	    break;

	case Magic('z'):
	    c = no_Magic(getchr());
	    switch (c)
	    {
		case 's':
		    EMIT(NFA_ZSTART);
		    if (re_mult_next("\\zs") == FAIL)
			return FAIL;
		    break;
		case 'e':
		    EMIT(NFA_ZEND);
		    rex.nfa_has_zend = TRUE;
		    if (re_mult_next("\\ze") == FAIL)
			return FAIL;
		    break;
#ifdef FEAT_SYN_HL
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		    // \z1...\z9
		    if ((reg_do_extmatch & REX_USE) == 0)
			EMSG_RET_FAIL(_(e_z1_not_allowed));
		    EMIT(NFA_ZREF1 + (no_Magic(c) - '1'));
		    // No need to set rex.nfa_has_backref, the sub-matches don't
		    // change when \z1 .. \z9 matches or not.
		    re_has_z = REX_USE;
		    break;
		case '(':
		    // \z(
		    if ((reg_do_extmatch & REX_SET) == 0)
			EMSG_RET_FAIL(_(e_z_not_allowed));
		    if (nfa_reg(REG_ZPAREN) == FAIL)
			return FAIL;	    // cascaded error
		    re_has_z = REX_SET;
		    break;
#endif
		default:
		    semsg(_("E867: (NFA) Unknown operator '\\z%c'"),
								 no_Magic(c));
		    return FAIL;
	    }
	    break;

	case Magic('%'):
	    c = no_Magic(getchr());
	    switch (c)
	    {
		// () without a back reference
		case '(':
		    if (nfa_reg(REG_NPAREN) == FAIL)
			return FAIL;
		    EMIT(NFA_NOPEN);
		    break;

		case 'd':   // %d123 decimal
		case 'o':   // %o123 octal
		case 'x':   // %xab hex 2
		case 'u':   // %uabcd hex 4
		case 'U':   // %U1234abcd hex 8
		    {
			long nr;

			switch (c)
			{
			    case 'd': nr = getdecchrs(); break;
			    case 'o': nr = getoctchrs(); break;
			    case 'x': nr = gethexchrs(2); break;
			    case 'u': nr = gethexchrs(4); break;
			    case 'U': nr = gethexchrs(8); break;
			    default:  nr = -1; break;
			}

			if (nr < 0 || nr > INT_MAX)
			    EMSG2_RET_FAIL(
			       _("E678: Invalid character after %s%%[dxouU]"),
				    reg_magic == MAGIC_ALL);
			// A NUL is stored in the text as NL
			// TODO: what if a composing character follows?
			EMIT(nr == 0 ? 0x0a : nr);
		    }
		    break;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		case '^':
		    EMIT(NFA_BOF);
		    break;

		case '$':
		    EMIT(NFA_EOF);
		    break;

		case '#':
		    EMIT(NFA_CURSOR);
		    break;

		case 'V':
		    EMIT(NFA_VISUAL);
		    break;

		case 'C':
		    EMIT(NFA_ANY_COMPOSING);
		    break;

		case '[':
		    {
			int	    n;

			// \%[abc]
			for (n = 0; (c = peekchr()) != ']'; ++n)
			{
			    if (c == NUL)
				EMSG2_RET_FAIL(_(e_missing_sb),
						      reg_magic == MAGIC_ALL);
			    // recursive call!
			    if (nfa_regatom() == FAIL)
				return FAIL;
			}
			getchr();  // get the ]
			if (n == 0)
			    EMSG2_RET_FAIL(_(e_empty_sb),
						      reg_magic == MAGIC_ALL);
			EMIT(NFA_OPT_CHARS);
			EMIT(n);

			// Emit as "\%(\%[abc]\)" to be able to handle
			// "\%[abc]*" which would cause the empty string to be
			// matched an unlimited number of times. NFA_NOPEN is
			// added only once at a position, while NFA_SPLIT is
			// added multiple times.  This is more efficient than
			// not allowing NFA_SPLIT multiple times, it is used
			// a lot.
			EMIT(NFA_NOPEN);
			break;
		    }

		default:
		    {
			long_u	n = 0;
			int	cmp = c;
			int	cur = FALSE;

			if (c == '<' || c == '>')
			    c = getchr();
			if (no_Magic(c) == '.')
			{
			    cur = TRUE;
			    c = getchr();
			}
			while (VIM_ISDIGIT(c))
			{
			    long_u tmp;

			    if (cur)
				semsg(_(e_regexp_number_after_dot_pos_search),
								 no_Magic(c));
			    tmp = n * 10 + (c - '0');

			    if (tmp < n)
			    {
				// overflow.
				emsg(_(e_value_too_large));
				return FAIL;
			    }
			    n = tmp;
			    c = getchr();
			}
			if (c == 'l' || c == 'c' || c == 'v')
			{
			    long_u limit = INT_MAX;

			    if (c == 'l')
			    {
				if (cur)
				    n = curwin->w_cursor.lnum;
				// \%{n}l  \%{n}<l  \%{n}>l
				EMIT(cmp == '<' ? NFA_LNUM_LT :
				     cmp == '>' ? NFA_LNUM_GT : NFA_LNUM);
				if (save_prev_at_start)
				    at_start = TRUE;
			    }
			    else if (c == 'c')
			    {
				if (cur)
				{
				    n = curwin->w_cursor.col;
				    n++;
				}
				// \%{n}c  \%{n}<c  \%{n}>c
				EMIT(cmp == '<' ? NFA_COL_LT :
				     cmp == '>' ? NFA_COL_GT : NFA_COL);
			    }
			    else
			    {
				if (cur)
				{
				    colnr_T vcol = 0;

				    getvvcol(curwin, &curwin->w_cursor,
							    NULL, NULL, &vcol);
				    n = ++vcol;
				}
				// \%{n}v  \%{n}<v  \%{n}>v
				EMIT(cmp == '<' ? NFA_VCOL_LT :
				     cmp == '>' ? NFA_VCOL_GT : NFA_VCOL);
				limit = INT_MAX / MB_MAXBYTES;
			    }
			    if (n >= limit)
			    {
				emsg(_(e_value_too_large));
				return FAIL;
			    }
			    EMIT((int)n);
			    break;
			}
			else if (c == '\'' && n == 0)
			{
			    // \%'m  \%<'m  \%>'m
			    EMIT(cmp == '<' ? NFA_MARK_LT :
				 cmp == '>' ? NFA_MARK_GT : NFA_MARK);
			    EMIT(getchr());
			    break;
			}
		    }
		    semsg(_("E867: (NFA) Unknown operator '\\%%%c'"),
								 no_Magic(c));
		    return FAIL;
	    }
	    break;

	case Magic('['):
collection:
	    /*
	     * [abc]  uses NFA_START_COLL - NFA_END_COLL
	     * [^abc] uses NFA_START_NEG_COLL - NFA_END_NEG_COLL
	     * Each character is produced as a regular state, using
	     * NFA_CONCAT to bind them together.
	     * Besides normal characters there can be:
	     * - character classes  NFA_CLASS_*
	     * - ranges, two characters followed by NFA_RANGE.
	     */

	    p = regparse;
	    endp = skip_anyof(p);
	    if (*endp == ']')
	    {
		/*
		 * Try to reverse engineer character classes. For example,
		 * recognize that [0-9] stands for \d and [A-Za-z_] for \h,
		 * and perform the necessary substitutions in the NFA.
		 */
		result = nfa_recognize_char_class(regparse, endp,
							 extra == NFA_ADD_NL);
		if (result != FAIL)
		{
		    if (result >= NFA_FIRST_NL && result <= NFA_LAST_NL)
		    {
			EMIT(result - NFA_ADD_NL);
			EMIT(NFA_NEWL);
			EMIT(NFA_OR);
		    }
		    else
			EMIT(result);
		    regparse = endp;
		    MB_PTR_ADV(regparse);
		    return OK;
		}
		/*
		 * Failed to recognize a character class. Use the simple
		 * version that turns [abc] into 'a' OR 'b' OR 'c'
		 */
		startc = -1;
		negated = FALSE;
		if (*regparse == '^')			// negated range
		{
		    negated = TRUE;
		    MB_PTR_ADV(regparse);
		    EMIT(NFA_START_NEG_COLL);
		}
		else
		    EMIT(NFA_START_COLL);
		if (*regparse == '-')
		{
		    startc = '-';
		    EMIT(startc);
		    EMIT(NFA_CONCAT);
		    MB_PTR_ADV(regparse);
		}
		// Emit the OR branches for each character in the []
		emit_range = FALSE;
		while (regparse < endp)
		{
		    int	    oldstartc = startc;

		    startc = -1;
		    got_coll_char = FALSE;
		    if (*regparse == '[')
		    {
			// Check for [: :], [= =], [. .]
			equiclass = collclass = 0;
			charclass = get_char_class(&regparse);
			if (charclass == CLASS_NONE)
			{
			    equiclass = get_equi_class(&regparse);
			    if (equiclass == 0)
				collclass = get_coll_element(&regparse);
			}

			// Character class like [:alpha:]
			if (charclass != CLASS_NONE)
			{
			    switch (charclass)
			    {
				case CLASS_ALNUM:
				    EMIT(NFA_CLASS_ALNUM);
				    break;
				case CLASS_ALPHA:
				    EMIT(NFA_CLASS_ALPHA);
				    break;
				case CLASS_BLANK:
				    EMIT(NFA_CLASS_BLANK);
				    break;
				case CLASS_CNTRL:
				    EMIT(NFA_CLASS_CNTRL);
				    break;
				case CLASS_DIGIT:
				    EMIT(NFA_CLASS_DIGIT);
				    break;
				case CLASS_GRAPH:
				    EMIT(NFA_CLASS_GRAPH);
				    break;
				case CLASS_LOWER:
				    wants_nfa = TRUE;
				    EMIT(NFA_CLASS_LOWER);
				    break;
				case CLASS_PRINT:
				    EMIT(NFA_CLASS_PRINT);
				    break;
				case CLASS_PUNCT:
				    EMIT(NFA_CLASS_PUNCT);
				    break;
				case CLASS_SPACE:
				    EMIT(NFA_CLASS_SPACE);
				    break;
				case CLASS_UPPER:
				    wants_nfa = TRUE;
				    EMIT(NFA_CLASS_UPPER);
				    break;
				case CLASS_XDIGIT:
				    EMIT(NFA_CLASS_XDIGIT);
				    break;
				case CLASS_TAB:
				    EMIT(NFA_CLASS_TAB);
				    break;
				case CLASS_RETURN:
				    EMIT(NFA_CLASS_RETURN);
				    break;
				case CLASS_BACKSPACE:
				    EMIT(NFA_CLASS_BACKSPACE);
				    break;
				case CLASS_ESCAPE:
				    EMIT(NFA_CLASS_ESCAPE);
				    break;
				case CLASS_IDENT:
				    EMIT(NFA_CLASS_IDENT);
				    break;
				case CLASS_KEYWORD:
				    EMIT(NFA_CLASS_KEYWORD);
				    break;
				case CLASS_FNAME:
				    EMIT(NFA_CLASS_FNAME);
				    break;
			    }
			    EMIT(NFA_CONCAT);
			    continue;
			}
			// Try equivalence class [=a=] and the like
			if (equiclass != 0)
			{
			    result = nfa_emit_equi_class(equiclass);
			    if (result == FAIL)
			    {
				// should never happen
				EMSG_RET_FAIL(_("E868: Error building NFA with equivalence class!"));
			    }
			    continue;
			}
			// Try collating class like [. .]
			if (collclass != 0)
			{
			    startc = collclass;	 // allow [.a.]-x as a range
			    // Will emit the proper atom at the end of the
			    // while loop.
			}
		    }
		    // Try a range like 'a-x' or '\t-z'. Also allows '-' as a
		    // start character.
		    if (*regparse == '-' && oldstartc != -1)
		    {
			emit_range = TRUE;
			startc = oldstartc;
			MB_PTR_ADV(regparse);
			continue;	    // reading the end of the range
		    }

		    // Now handle simple and escaped characters.
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    if (*regparse == '\\'
			    && !reg_cpo_bsl
			    && regparse + 1 <= endp
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != NULL
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR, regparse[1])
								      != NULL)
			    )
			)
		    {
			MB_PTR_ADV(regparse);

			if (*regparse == 'n')
			    startc = (reg_string || emit_range
					|| regparse[1] == '-') ? NL : NFA_NEWL;
			else if (*regparse == 'd'
				    || *regparse == 'o'
				    || *regparse == 'x'
				    || *regparse == 'u'
				    || *regparse == 'U'
				)
			    {
				// TODO(RE) This needs more testing
				startc = coll_get_char();
				got_coll_char = TRUE;
				MB_PTR_BACK(old_regparse, regparse);
			    }
			    else
			    {
				// \r,\t,\e,\b
				startc = backslash_trans(*regparse);
			    }
		    }

		    // Normal printable char
		    if (startc == -1)
			startc = PTR2CHAR(regparse);

		    // Previous char was '-', so this char is end of range.
		    if (emit_range)
		    {
			int	endc = startc;

			startc = oldstartc;
			if (startc > endc)
			    EMSG_RET_FAIL(_(e_reverse_range));

			if (endc > startc + 2)
			{
			    // Emit a range instead of the sequence of
			    // individual characters.
			    if (startc == 0)
				// \x00 is translated to \x0a, start at \x01.
				EMIT(1);
			    else
				--post_ptr; // remove NFA_CONCAT
			    EMIT(endc);
			    EMIT(NFA_RANGE);
			    EMIT(NFA_CONCAT);
			}
			else if (has_mbyte && ((*mb_char2len)(startc) > 1
				    || (*mb_char2len)(endc) > 1))
			{
			    // Emit the characters in the range.
			    // "startc" was already emitted, so skip it.
			    //
			    for (c = startc + 1; c <= endc; c++)
			    {
				EMIT(c);
				EMIT(NFA_CONCAT);
			    }
			}
			else
			{
#ifdef EBCDIC
			    int alpha_only = FALSE;

			    // for alphabetical range skip the gaps
			    // 'i'-'j', 'r'-'s', 'I'-'J' and 'R'-'S'.
			    if (isalpha(startc) && isalpha(endc))
				alpha_only = TRUE;
#endif
			    // Emit the range. "startc" was already emitted, so
			    // skip it.
			    for (c = startc + 1; c <= endc; c++)
#ifdef EBCDIC
				if (!alpha_only || isalpha(startc))
#endif
				{
				    EMIT(c);
				    EMIT(NFA_CONCAT);
				}
			}
			emit_range = FALSE;
			startc = -1;
		    }
		    else
		    {
			// This char (startc) is not part of a range. Just
			// emit it.
			// Normally, simply emit startc. But if we get char
			// code=0 from a collating char, then replace it with
			// 0x0a.
			// This is needed to completely mimic the behaviour of
			// the backtracking engine.
			if (startc == NFA_NEWL)
			{
			    // Line break can't be matched as part of the
			    // collection, add an OR below. But not for negated
			    // range.
			    if (!negated)
				extra = NFA_ADD_NL;
			}
			else
			{
			    if (got_coll_char == TRUE && startc == 0)
				EMIT(0x0a);
			    else
				EMIT(startc);
			    EMIT(NFA_CONCAT);
			}
		    }

		    MB_PTR_ADV(regparse);
		} // while (p < endp)

		MB_PTR_BACK(old_regparse, regparse);
		if (*regparse == '-')	    // if last, '-' is just a char
		{
		    EMIT('-');
		    EMIT(NFA_CONCAT);
		}

		// skip the trailing ]
		regparse = endp;
		MB_PTR_ADV(regparse);

		// Mark end of the collection.
		if (negated == TRUE)
		    EMIT(NFA_END_NEG_COLL);
		else
		    EMIT(NFA_END_COLL);

		// \_[] also matches \n but it's not negated
		if (extra == NFA_ADD_NL)
		{
		    EMIT(reg_string ? NL : NFA_NEWL);
		    EMIT(NFA_OR);
		}

		return OK;
	    } // if exists closing ]

	    if (reg_strict)
		EMSG_RET_FAIL(_(e_missingbracket));
	    // FALLTHROUGH

	default:
	    {
		int	plen;

nfa_do_multibyte:
		// plen is length of current char with composing chars
		if (enc_utf8 && ((*mb_char2len)(c)
			    != (plen = utfc_ptr2len(old_regparse))
						       || utf_iscomposing(c)))
		{
		    int i = 0;

		    // A base character plus composing characters, or just one
		    // or more composing characters.
		    // This requires creating a separate atom as if enclosing
		    // the characters in (), where NFA_COMPOSING is the ( and
		    // NFA_END_COMPOSING is the ). Note that right now we are
		    // building the postfix form, not the NFA itself;
		    // a composing char could be: a, b, c, NFA_COMPOSING
		    // where 'b' and 'c' are chars with codes > 256.
		    for (;;)
		    {
			EMIT(c);
			if (i > 0)
			    EMIT(NFA_CONCAT);
			if ((i += utf_char2len(c)) >= plen)
			    break;
			c = utf_ptr2char(old_regparse + i);
		    }
		    EMIT(NFA_COMPOSING);
		    regparse = old_regparse + plen;
		}
		else
		{
		    c = no_Magic(c);
		    EMIT(c);
		}
		return OK;
	    }
    }

    return OK;
}