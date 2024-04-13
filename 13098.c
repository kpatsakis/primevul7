regrepeat(
    char_u	*p,
    long	maxcount)   // maximum number of matches allowed
{
    long	count = 0;
    char_u	*scan;
    char_u	*opnd;
    int		mask;
    int		testval = 0;

    scan = rex.input;	    // Make local copy of rex.input for speed.
    opnd = OPERAND(p);
    switch (OP(p))
    {
      case ANY:
      case ANY + ADD_NL:
	while (count < maxcount)
	{
	    // Matching anything means we continue until end-of-line (or
	    // end-of-file for ANY + ADD_NL), only limited by maxcount.
	    while (*scan != NUL && count < maxcount)
	    {
		++count;
		MB_PTR_ADV(scan);
	    }
	    if (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
				      || rex.reg_line_lbr || count == maxcount)
		break;
	    ++count;		// count the line-break
	    reg_nextline();
	    scan = rex.input;
	    if (got_int)
		break;
	}
	break;

      case IDENT:
      case IDENT + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      case SIDENT:
      case SIDENT + ADD_NL:
	while (count < maxcount)
	{
	    if (vim_isIDc(PTR2CHAR(scan)) && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    else if (*scan == NUL)
	    {
		if (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_line_lbr)
		    break;
		reg_nextline();
		scan = rex.input;
		if (got_int)
		    break;
	    }
	    else if (rex.reg_line_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    else
		break;
	    ++count;
	}
	break;

      case KWORD:
      case KWORD + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      case SKWORD:
      case SKWORD + ADD_NL:
	while (count < maxcount)
	{
	    if (vim_iswordp_buf(scan, rex.reg_buf)
					  && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    else if (*scan == NUL)
	    {
		if (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_line_lbr)
		    break;
		reg_nextline();
		scan = rex.input;
		if (got_int)
		    break;
	    }
	    else if (rex.reg_line_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    else
		break;
	    ++count;
	}
	break;

      case FNAME:
      case FNAME + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      case SFNAME:
      case SFNAME + ADD_NL:
	while (count < maxcount)
	{
	    if (vim_isfilec(PTR2CHAR(scan)) && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    else if (*scan == NUL)
	    {
		if (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_line_lbr)
		    break;
		reg_nextline();
		scan = rex.input;
		if (got_int)
		    break;
	    }
	    else if (rex.reg_line_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    else
		break;
	    ++count;
	}
	break;

      case PRINT:
      case PRINT + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH
      case SPRINT:
      case SPRINT + ADD_NL:
	while (count < maxcount)
	{
	    if (*scan == NUL)
	    {
		if (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_line_lbr)
		    break;
		reg_nextline();
		scan = rex.input;
		if (got_int)
		    break;
	    }
	    else if (vim_isprintc(PTR2CHAR(scan)) == 1
					  && (testval || !VIM_ISDIGIT(*scan)))
	    {
		MB_PTR_ADV(scan);
	    }
	    else if (rex.reg_line_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    else
		break;
	    ++count;
	}
	break;

      case WHITE:
      case WHITE + ADD_NL:
	testval = mask = RI_WHITE;
do_class:
	while (count < maxcount)
	{
	    int		l;

	    if (*scan == NUL)
	    {
		if (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_line_lbr)
		    break;
		reg_nextline();
		scan = rex.input;
		if (got_int)
		    break;
	    }
	    else if (has_mbyte && (l = (*mb_ptr2len)(scan)) > 1)
	    {
		if (testval != 0)
		    break;
		scan += l;
	    }
	    else if ((class_tab[*scan] & mask) == testval)
		++scan;
	    else if (rex.reg_line_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    else
		break;
	    ++count;
	}
	break;

      case NWHITE:
      case NWHITE + ADD_NL:
	mask = RI_WHITE;
	goto do_class;
      case DIGIT:
      case DIGIT + ADD_NL:
	testval = mask = RI_DIGIT;
	goto do_class;
      case NDIGIT:
      case NDIGIT + ADD_NL:
	mask = RI_DIGIT;
	goto do_class;
      case HEX:
      case HEX + ADD_NL:
	testval = mask = RI_HEX;
	goto do_class;
      case NHEX:
      case NHEX + ADD_NL:
	mask = RI_HEX;
	goto do_class;
      case OCTAL:
      case OCTAL + ADD_NL:
	testval = mask = RI_OCTAL;
	goto do_class;
      case NOCTAL:
      case NOCTAL + ADD_NL:
	mask = RI_OCTAL;
	goto do_class;
      case WORD:
      case WORD + ADD_NL:
	testval = mask = RI_WORD;
	goto do_class;
      case NWORD:
      case NWORD + ADD_NL:
	mask = RI_WORD;
	goto do_class;
      case HEAD:
      case HEAD + ADD_NL:
	testval = mask = RI_HEAD;
	goto do_class;
      case NHEAD:
      case NHEAD + ADD_NL:
	mask = RI_HEAD;
	goto do_class;
      case ALPHA:
      case ALPHA + ADD_NL:
	testval = mask = RI_ALPHA;
	goto do_class;
      case NALPHA:
      case NALPHA + ADD_NL:
	mask = RI_ALPHA;
	goto do_class;
      case LOWER:
      case LOWER + ADD_NL:
	testval = mask = RI_LOWER;
	goto do_class;
      case NLOWER:
      case NLOWER + ADD_NL:
	mask = RI_LOWER;
	goto do_class;
      case UPPER:
      case UPPER + ADD_NL:
	testval = mask = RI_UPPER;
	goto do_class;
      case NUPPER:
      case NUPPER + ADD_NL:
	mask = RI_UPPER;
	goto do_class;

      case EXACTLY:
	{
	    int	    cu, cl;

	    // This doesn't do a multi-byte character, because a MULTIBYTECODE
	    // would have been used for it.  It does handle single-byte
	    // characters, such as latin1.
	    if (rex.reg_ic)
	    {
		cu = MB_TOUPPER(*opnd);
		cl = MB_TOLOWER(*opnd);
		while (count < maxcount && (*scan == cu || *scan == cl))
		{
		    count++;
		    scan++;
		}
	    }
	    else
	    {
		cu = *opnd;
		while (count < maxcount && *scan == cu)
		{
		    count++;
		    scan++;
		}
	    }
	    break;
	}

      case MULTIBYTECODE:
	{
	    int		i, len, cf = 0;

	    // Safety check (just in case 'encoding' was changed since
	    // compiling the program).
	    if ((len = (*mb_ptr2len)(opnd)) > 1)
	    {
		if (rex.reg_ic && enc_utf8)
		    cf = utf_fold(utf_ptr2char(opnd));
		while (count < maxcount && (*mb_ptr2len)(scan) >= len)
		{
		    for (i = 0; i < len; ++i)
			if (opnd[i] != scan[i])
			    break;
		    if (i < len && (!rex.reg_ic || !enc_utf8
					|| utf_fold(utf_ptr2char(scan)) != cf))
			break;
		    scan += len;
		    ++count;
		}
	    }
	}
	break;

      case ANYOF:
      case ANYOF + ADD_NL:
	testval = TRUE;
	// FALLTHROUGH

      case ANYBUT:
      case ANYBUT + ADD_NL:
	while (count < maxcount)
	{
	    int len;

	    if (*scan == NUL)
	    {
		if (!REG_MULTI || !WITH_NL(OP(p)) || rex.lnum > rex.reg_maxline
							   || rex.reg_line_lbr)
		    break;
		reg_nextline();
		scan = rex.input;
		if (got_int)
		    break;
	    }
	    else if (rex.reg_line_lbr && *scan == '\n' && WITH_NL(OP(p)))
		++scan;
	    else if (has_mbyte && (len = (*mb_ptr2len)(scan)) > 1)
	    {
		if ((cstrchr(opnd, (*mb_ptr2char)(scan)) == NULL) == testval)
		    break;
		scan += len;
	    }
	    else
	    {
		if ((cstrchr(opnd, *scan) == NULL) == testval)
		    break;
		++scan;
	    }
	    ++count;
	}
	break;

      case NEWL:
	while (count < maxcount
		&& ((*scan == NUL && rex.lnum <= rex.reg_maxline
				       && !rex.reg_line_lbr && REG_MULTI)
		    || (*scan == '\n' && rex.reg_line_lbr)))
	{
	    count++;
	    if (rex.reg_line_lbr)
		ADVANCE_REGINPUT();
	    else
		reg_nextline();
	    scan = rex.input;
	    if (got_int)
		break;
	}
	break;

      default:			// Oh dear.  Called inappropriately.
	iemsg(_(e_corrupted_regexp_program));
#ifdef DEBUG
	printf("Called regrepeat with op code %d\n", OP(p));
#endif
	break;
    }

    rex.input = scan;

    return (int)count;
}