regatom(int *flagp)
{
    char_u	    *ret;
    int		    flags;
    int		    c;
    char_u	    *p;
    int		    extra = 0;
    int		    save_prev_at_start = prev_at_start;

    *flagp = WORST;		// Tentatively.

    c = getchr();
    switch (c)
    {
      case Magic('^'):
	ret = regnode(BOL);
	break;

      case Magic('$'):
	ret = regnode(EOL);
#if defined(FEAT_SYN_HL) || defined(PROTO)
	had_eol = TRUE;
#endif
	break;

      case Magic('<'):
	ret = regnode(BOW);
	break;

      case Magic('>'):
	ret = regnode(EOW);
	break;

      case Magic('_'):
	c = no_Magic(getchr());
	if (c == '^')		// "\_^" is start-of-line
	{
	    ret = regnode(BOL);
	    break;
	}
	if (c == '$')		// "\_$" is end-of-line
	{
	    ret = regnode(EOL);
#if defined(FEAT_SYN_HL) || defined(PROTO)
	    had_eol = TRUE;
#endif
	    break;
	}

	extra = ADD_NL;
	*flagp |= HASNL;

	// "\_[" is character range plus newline
	if (c == '[')
	    goto collection;

	// "\_x" is character class plus newline
	// FALLTHROUGH

	// Character classes.
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
	    EMSG_RET_NULL(_(e_invalid_use_of_underscore));

	// When '.' is followed by a composing char ignore the dot, so that
	// the composing char is matched here.
	if (enc_utf8 && c == Magic('.') && utf_iscomposing(peekchr()))
	{
	    c = getchr();
	    goto do_multibyte;
	}
	ret = regnode(classcodes[p - classchars] + extra);
	*flagp |= HASWIDTH | SIMPLE;
	break;

      case Magic('n'):
	if (reg_string)
	{
	    // In a string "\n" matches a newline character.
	    ret = regnode(EXACTLY);
	    regc(NL);
	    regc(NUL);
	    *flagp |= HASWIDTH | SIMPLE;
	}
	else
	{
	    // In buffer text "\n" matches the end of a line.
	    ret = regnode(NEWL);
	    *flagp |= HASWIDTH | HASNL;
	}
	break;

      case Magic('('):
	if (one_exactly)
	    EMSG_ONE_RET_NULL;
	ret = reg(REG_PAREN, &flags);
	if (ret == NULL)
	    return NULL;
	*flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
	break;

      case NUL:
      case Magic('|'):
      case Magic('&'):
      case Magic(')'):
	if (one_exactly)
	    EMSG_ONE_RET_NULL;
	// Supposed to be caught earlier.
	IEMSG_RET_NULL(_(e_internal_error_in_regexp));
	// NOTREACHED

      case Magic('='):
      case Magic('?'):
      case Magic('+'):
      case Magic('@'):
      case Magic('{'):
      case Magic('*'):
	c = no_Magic(c);
	EMSG3_RET_NULL(_(e_str_chr_follows_nothing),
		(c == '*' ? reg_magic >= MAGIC_ON : reg_magic == MAGIC_ALL), c);
	// NOTREACHED

      case Magic('~'):		// previous substitute pattern
	    if (reg_prev_sub != NULL)
	    {
		char_u	    *lp;

		ret = regnode(EXACTLY);
		lp = reg_prev_sub;
		while (*lp != NUL)
		    regc(*lp++);
		regc(NUL);
		if (*reg_prev_sub != NUL)
		{
		    *flagp |= HASWIDTH;
		    if ((lp - reg_prev_sub) == 1)
			*flagp |= SIMPLE;
		}
	    }
	    else
		EMSG_RET_NULL(_(e_no_previous_substitute_regular_expression));
	    break;

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
		int		    refnum;

		refnum = c - Magic('0');
		if (!seen_endbrace(refnum))
		    return NULL;
		ret = regnode(BACKREF + refnum);
	    }
	    break;

      case Magic('z'):
	{
	    c = no_Magic(getchr());
	    switch (c)
	    {
#ifdef FEAT_SYN_HL
		case '(': if ((reg_do_extmatch & REX_SET) == 0)
			      EMSG_RET_NULL(_(e_z_not_allowed_here));
			  if (one_exactly)
			      EMSG_ONE_RET_NULL;
			  ret = reg(REG_ZPAREN, &flags);
			  if (ret == NULL)
			      return NULL;
			  *flagp |= flags & (HASWIDTH|SPSTART|HASNL|HASLOOKBH);
			  re_has_z = REX_SET;
			  break;

		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': if ((reg_do_extmatch & REX_USE) == 0)
			      EMSG_RET_NULL(_(e_z1_z9_not_allowed_here));
			  ret = regnode(ZREF + c - '0');
			  re_has_z = REX_USE;
			  break;
#endif

		case 's': ret = regnode(MOPEN + 0);
			  if (re_mult_next("\\zs") == FAIL)
			      return NULL;
			  break;

		case 'e': ret = regnode(MCLOSE + 0);
			  if (re_mult_next("\\ze") == FAIL)
			      return NULL;
			  break;

		default:  EMSG_RET_NULL(_(e_invalid_character_after_bsl_z));
	    }
	}
	break;

      case Magic('%'):
	{
	    c = no_Magic(getchr());
	    switch (c)
	    {
		// () without a back reference
		case '(':
		    if (one_exactly)
			EMSG_ONE_RET_NULL;
		    ret = reg(REG_NPAREN, &flags);
		    if (ret == NULL)
			return NULL;
		    *flagp |= flags & (HASWIDTH | SPSTART | HASNL | HASLOOKBH);
		    break;

		// Catch \%^ and \%$ regardless of where they appear in the
		// pattern -- regardless of whether or not it makes sense.
		case '^':
		    ret = regnode(RE_BOF);
		    break;

		case '$':
		    ret = regnode(RE_EOF);
		    break;

		case '#':
		    ret = regnode(CURSOR);
		    break;

		case 'V':
		    ret = regnode(RE_VISUAL);
		    break;

		case 'C':
		    ret = regnode(RE_COMPOSING);
		    break;

		// \%[abc]: Emit as a list of branches, all ending at the last
		// branch which matches nothing.
		case '[':
			  if (one_exactly)	// doesn't nest
			      EMSG_ONE_RET_NULL;
			  {
			      char_u	*lastbranch;
			      char_u	*lastnode = NULL;
			      char_u	*br;

			      ret = NULL;
			      while ((c = getchr()) != ']')
			      {
				  if (c == NUL)
				      EMSG2_RET_NULL(_(e_missing_sb_after_str),
						      reg_magic == MAGIC_ALL);
				  br = regnode(BRANCH);
				  if (ret == NULL)
				      ret = br;
				  else
				  {
				      regtail(lastnode, br);
				      if (reg_toolong)
					  return NULL;
				  }

				  ungetchr();
				  one_exactly = TRUE;
				  lastnode = regatom(flagp);
				  one_exactly = FALSE;
				  if (lastnode == NULL)
				      return NULL;
			      }
			      if (ret == NULL)
				  EMSG2_RET_NULL(_(e_empty_str_brackets),
						      reg_magic == MAGIC_ALL);
			      lastbranch = regnode(BRANCH);
			      br = regnode(NOTHING);
			      if (ret != JUST_CALC_SIZE)
			      {
				  regtail(lastnode, br);
				  regtail(lastbranch, br);
				  // connect all branches to the NOTHING
				  // branch at the end
				  for (br = ret; br != lastnode; )
				  {
				      if (OP(br) == BRANCH)
				      {
					  regtail(br, lastbranch);
					  if (reg_toolong)
					      return NULL;
					  br = OPERAND(br);
				      }
				      else
					  br = regnext(br);
				  }
			      }
			      *flagp &= ~(HASWIDTH | SIMPLE);
			      break;
			  }

		case 'd':   // %d123 decimal
		case 'o':   // %o123 octal
		case 'x':   // %xab hex 2
		case 'u':   // %uabcd hex 4
		case 'U':   // %U1234abcd hex 8
			  {
			      long i;

			      switch (c)
			      {
				  case 'd': i = getdecchrs(); break;
				  case 'o': i = getoctchrs(); break;
				  case 'x': i = gethexchrs(2); break;
				  case 'u': i = gethexchrs(4); break;
				  case 'U': i = gethexchrs(8); break;
				  default:  i = -1; break;
			      }

			      if (i < 0 || i > INT_MAX)
				  EMSG2_RET_NULL(
					    _(e_invalid_character_after_str_2),
						       reg_magic == MAGIC_ALL);
			      if (use_multibytecode(i))
				  ret = regnode(MULTIBYTECODE);
			      else
				  ret = regnode(EXACTLY);
			      if (i == 0)
				  regc(0x0a);
			      else
				  regmbc(i);
			      regc(NUL);
			      *flagp |= HASWIDTH;
			      break;
			  }

		default:
			  if (VIM_ISDIGIT(c) || c == '<' || c == '>'
						|| c == '\'' || c == '.')
			  {
			      long_u	n = 0;
			      int	cmp;
			      int	cur = FALSE;

			      cmp = c;
			      if (cmp == '<' || cmp == '>')
				  c = getchr();
			      if (no_Magic(c) == '.')
			      {
				  cur = TRUE;
				  c = getchr();
			      }
			      while (VIM_ISDIGIT(c))
			      {
				  n = n * 10 + (c - '0');
				  c = getchr();
			      }
			      if (c == '\'' && n == 0)
			      {
				  // "\%'m", "\%<'m" and "\%>'m": Mark
				  c = getchr();
				  ret = regnode(RE_MARK);
				  if (ret == JUST_CALC_SIZE)
				      regsize += 2;
				  else
				  {
				      *regcode++ = c;
				      *regcode++ = cmp;
				  }
				  break;
			      }
			      else if (c == 'l' || c == 'c' || c == 'v')
			      {
				  if (cur && n)
				  {
				    semsg(_(e_regexp_number_after_dot_pos_search), no_Magic(c));
				    rc_did_emsg = TRUE;
				    return NULL;
				  }
				  if (c == 'l')
				  {
				      if (cur)
					  n = curwin->w_cursor.lnum;
				      ret = regnode(RE_LNUM);
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
				      ret = regnode(RE_COL);
				  }
				  else
				  {
				      if (cur)
				      {
					  colnr_T vcol = 0;

					  getvvcol(curwin, &curwin->w_cursor,
							    NULL, NULL, &vcol);
					  ++vcol;
					  n = vcol;
				      }
				      ret = regnode(RE_VCOL);
				  }
				  if (ret == JUST_CALC_SIZE)
				      regsize += 5;
				  else
				  {
				      // put the number and the optional
				      // comparator after the opcode
				      regcode = re_put_long(regcode, n);
				      *regcode++ = cmp;
				  }
				  break;
			      }
			  }

			  EMSG2_RET_NULL(_(e_invalid_character_after_str),
						      reg_magic == MAGIC_ALL);
	    }
	}
	break;

      case Magic('['):
collection:
	{
	    char_u	*lp;

	    // If there is no matching ']', we assume the '[' is a normal
	    // character.  This makes 'incsearch' and ":help [" work.
	    lp = skip_anyof(regparse);
	    if (*lp == ']')	// there is a matching ']'
	    {
		int	startc = -1;	// > 0 when next '-' is a range
		int	endc;

		// In a character class, different parsing rules apply.
		// Not even \ is special anymore, nothing is.
		if (*regparse == '^')	    // Complement of range.
		{
		    ret = regnode(ANYBUT + extra);
		    regparse++;
		}
		else
		    ret = regnode(ANYOF + extra);

		// At the start ']' and '-' mean the literal character.
		if (*regparse == ']' || *regparse == '-')
		{
		    startc = *regparse;
		    regc(*regparse++);
		}

		while (*regparse != NUL && *regparse != ']')
		{
		    if (*regparse == '-')
		    {
			++regparse;
			// The '-' is not used for a range at the end and
			// after or before a '\n'.
			if (*regparse == ']' || *regparse == NUL
				|| startc == -1
				|| (regparse[0] == '\\' && regparse[1] == 'n'))
			{
			    regc('-');
			    startc = '-';	// [--x] is a range
			}
			else
			{
			    // Also accept "a-[.z.]"
			    endc = 0;
			    if (*regparse == '[')
				endc = get_coll_element(&regparse);
			    if (endc == 0)
			    {
				if (has_mbyte)
				    endc = mb_ptr2char_adv(&regparse);
				else
				    endc = *regparse++;
			    }

			    // Handle \o40, \x20 and \u20AC style sequences
			    if (endc == '\\' && !reg_cpo_lit && !reg_cpo_bsl)
				endc = coll_get_char();

			    if (startc > endc)
				EMSG_RET_NULL(_(e_reverse_range_in_character_class));
			    if (has_mbyte && ((*mb_char2len)(startc) > 1
						 || (*mb_char2len)(endc) > 1))
			    {
				// Limit to a range of 256 chars.
				if (endc > startc + 256)
				    EMSG_RET_NULL(_(e_range_too_large_in_character_class));
				while (++startc <= endc)
				    regmbc(startc);
			    }
			    else
			    {
				while (++startc <= endc)
				    regc(startc);
			    }
			    startc = -1;
			}
		    }
		    // Only "\]", "\^", "\]" and "\\" are special in Vi.  Vim
		    // accepts "\t", "\e", etc., but only when the 'l' flag in
		    // 'cpoptions' is not included.
		    // Posix doesn't recognize backslash at all.
		    else if (*regparse == '\\'
			    && !reg_cpo_bsl
			    && (vim_strchr(REGEXP_INRANGE, regparse[1]) != NULL
				|| (!reg_cpo_lit
				    && vim_strchr(REGEXP_ABBR,
						       regparse[1]) != NULL)))
		    {
			regparse++;
			if (*regparse == 'n')
			{
			    // '\n' in range: also match NL
			    if (ret != JUST_CALC_SIZE)
			    {
				// Using \n inside [^] does not change what
				// matches. "[^\n]" is the same as ".".
				if (*ret == ANYOF)
				{
				    *ret = ANYOF + ADD_NL;
				    *flagp |= HASNL;
				}
				// else: must have had a \n already
			    }
			    regparse++;
			    startc = -1;
			}
			else if (*regparse == 'd'
				|| *regparse == 'o'
				|| *regparse == 'x'
				|| *regparse == 'u'
				|| *regparse == 'U')
			{
			    startc = coll_get_char();
			    if (startc == 0)
				regc(0x0a);
			    else
				regmbc(startc);
			}
			else
			{
			    startc = backslash_trans(*regparse++);
			    regc(startc);
			}
		    }
		    else if (*regparse == '[')
		    {
			int c_class;
			int cu;

			c_class = get_char_class(&regparse);
			startc = -1;
			// Characters assumed to be 8 bits!
			switch (c_class)
			{
			    case CLASS_NONE:
				c_class = get_equi_class(&regparse);
				if (c_class != 0)
				{
				    // produce equivalence class
				    reg_equi_class(c_class);
				}
				else if ((c_class =
					    get_coll_element(&regparse)) != 0)
				{
				    // produce a collating element
				    regmbc(c_class);
				}
				else
				{
				    // literal '[', allow [[-x] as a range
				    startc = *regparse++;
				    regc(startc);
				}
				break;
			    case CLASS_ALNUM:
				for (cu = 1; cu < 128; cu++)
				    if (isalnum(cu))
					regmbc(cu);
				break;
			    case CLASS_ALPHA:
				for (cu = 1; cu < 128; cu++)
				    if (isalpha(cu))
					regmbc(cu);
				break;
			    case CLASS_BLANK:
				regc(' ');
				regc('\t');
				break;
			    case CLASS_CNTRL:
				for (cu = 1; cu <= 127; cu++)
				    if (iscntrl(cu))
					regmbc(cu);
				break;
			    case CLASS_DIGIT:
				for (cu = 1; cu <= 127; cu++)
				    if (VIM_ISDIGIT(cu))
					regmbc(cu);
				break;
			    case CLASS_GRAPH:
				for (cu = 1; cu <= 127; cu++)
				    if (isgraph(cu))
					regmbc(cu);
				break;
			    case CLASS_LOWER:
				for (cu = 1; cu <= 255; cu++)
				    if (MB_ISLOWER(cu) && cu != 170
								 && cu != 186)
					regmbc(cu);
				break;
			    case CLASS_PRINT:
				for (cu = 1; cu <= 255; cu++)
				    if (vim_isprintc(cu))
					regmbc(cu);
				break;
			    case CLASS_PUNCT:
				for (cu = 1; cu < 128; cu++)
				    if (ispunct(cu))
					regmbc(cu);
				break;
			    case CLASS_SPACE:
				for (cu = 9; cu <= 13; cu++)
				    regc(cu);
				regc(' ');
				break;
			    case CLASS_UPPER:
				for (cu = 1; cu <= 255; cu++)
				    if (MB_ISUPPER(cu))
					regmbc(cu);
				break;
			    case CLASS_XDIGIT:
				for (cu = 1; cu <= 255; cu++)
				    if (vim_isxdigit(cu))
					regmbc(cu);
				break;
			    case CLASS_TAB:
				regc('\t');
				break;
			    case CLASS_RETURN:
				regc('\r');
				break;
			    case CLASS_BACKSPACE:
				regc('\b');
				break;
			    case CLASS_ESCAPE:
				regc('\033');
				break;
			    case CLASS_IDENT:
				for (cu = 1; cu <= 255; cu++)
				    if (vim_isIDc(cu))
					regmbc(cu);
				break;
			    case CLASS_KEYWORD:
				for (cu = 1; cu <= 255; cu++)
				    if (reg_iswordc(cu))
					regmbc(cu);
				break;
			    case CLASS_FNAME:
				for (cu = 1; cu <= 255; cu++)
				    if (vim_isfilec(cu))
					regmbc(cu);
				break;
			}
		    }
		    else
		    {
			if (has_mbyte)
			{
			    int	len;

			    // produce a multibyte character, including any
			    // following composing characters
			    startc = mb_ptr2char(regparse);
			    len = (*mb_ptr2len)(regparse);
			    if (enc_utf8 && utf_char2len(startc) != len)
				startc = -1;	// composing chars
			    while (--len >= 0)
				regc(*regparse++);
			}
			else
			{
			    startc = *regparse++;
			    regc(startc);
			}
		    }
		}
		regc(NUL);
		prevchr_len = 1;	// last char was the ']'
		if (*regparse != ']')
		    EMSG_RET_NULL(_(e_too_many_brackets));  // Cannot happen?
		skipchr();	    // let's be friends with the lexer again
		*flagp |= HASWIDTH | SIMPLE;
		break;
	    }
	    else if (reg_strict)
		EMSG2_RET_NULL(_(e_missing_rsb_after_str_lsb),
							reg_magic > MAGIC_OFF);
	}
	// FALLTHROUGH

      default:
	{
	    int		len;

	    // A multi-byte character is handled as a separate atom if it's
	    // before a multi and when it's a composing char.
	    if (use_multibytecode(c))
	    {
do_multibyte:
		ret = regnode(MULTIBYTECODE);
		regmbc(c);
		*flagp |= HASWIDTH | SIMPLE;
		break;
	    }

	    ret = regnode(EXACTLY);

	    // Append characters as long as:
	    // - there is no following multi, we then need the character in
	    //   front of it as a single character operand
	    // - not running into a Magic character
	    // - "one_exactly" is not set
	    // But always emit at least one character.  Might be a Multi,
	    // e.g., a "[" without matching "]".
	    for (len = 0; c != NUL && (len == 0
			|| (re_multi_type(peekchr()) == NOT_MULTI
			    && !one_exactly
			    && !is_Magic(c))); ++len)
	    {
		c = no_Magic(c);
		if (has_mbyte)
		{
		    regmbc(c);
		    if (enc_utf8)
		    {
			int	l;

			// Need to get composing character too.
			for (;;)
			{
			    l = utf_ptr2len(regparse);
			    if (!UTF_COMPOSINGLIKE(regparse, regparse + l))
				break;
			    regmbc(utf_ptr2char(regparse));
			    skipchr();
			}
		    }
		}
		else
		    regc(c);
		c = getchr();
	    }
	    ungetchr();

	    regc(NUL);
	    *flagp |= HASWIDTH;
	    if (len == 1)
		*flagp |= SIMPLE;
	}
	break;
    }

    return ret;
}