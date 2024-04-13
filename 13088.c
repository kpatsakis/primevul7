bt_regexec_both(
    char_u	*line,
    colnr_T	col,		// column to start looking for match
    proftime_T	*tm,		// timeout limit or NULL
    int		*timed_out)	// flag set on timeout or NULL
{
    bt_regprog_T    *prog;
    char_u	    *s;
    long	    retval = 0L;

    // Create "regstack" and "backpos" if they are not allocated yet.
    // We allocate *_INITIAL amount of bytes first and then set the grow size
    // to much bigger value to avoid many malloc calls in case of deep regular
    // expressions.
    if (regstack.ga_data == NULL)
    {
	// Use an item size of 1 byte, since we push different things
	// onto the regstack.
	ga_init2(&regstack, 1, REGSTACK_INITIAL);
	(void)ga_grow(&regstack, REGSTACK_INITIAL);
	regstack.ga_growsize = REGSTACK_INITIAL * 8;
    }

    if (backpos.ga_data == NULL)
    {
	ga_init2(&backpos, sizeof(backpos_T), BACKPOS_INITIAL);
	(void)ga_grow(&backpos, BACKPOS_INITIAL);
	backpos.ga_growsize = BACKPOS_INITIAL * 8;
    }

    if (REG_MULTI)
    {
	prog = (bt_regprog_T *)rex.reg_mmatch->regprog;
	line = reg_getline((linenr_T)0);
	rex.reg_startpos = rex.reg_mmatch->startpos;
	rex.reg_endpos = rex.reg_mmatch->endpos;
    }
    else
    {
	prog = (bt_regprog_T *)rex.reg_match->regprog;
	rex.reg_startp = rex.reg_match->startp;
	rex.reg_endp = rex.reg_match->endp;
    }

    // Be paranoid...
    if (prog == NULL || line == NULL)
    {
	iemsg(_(e_null_argument));
	goto theend;
    }

    // Check validity of program.
    if (prog_magic_wrong())
	goto theend;

    // If the start column is past the maximum column: no need to try.
    if (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	goto theend;

    // If pattern contains "\c" or "\C": overrule value of rex.reg_ic
    if (prog->regflags & RF_ICASE)
	rex.reg_ic = TRUE;
    else if (prog->regflags & RF_NOICASE)
	rex.reg_ic = FALSE;

    // If pattern contains "\Z" overrule value of rex.reg_icombine
    if (prog->regflags & RF_ICOMBINE)
	rex.reg_icombine = TRUE;

    // If there is a "must appear" string, look for it.
    if (prog->regmust != NULL)
    {
	int c;

	if (has_mbyte)
	    c = (*mb_ptr2char)(prog->regmust);
	else
	    c = *prog->regmust;
	s = line + col;

	// This is used very often, esp. for ":global".  Use three versions of
	// the loop to avoid overhead of conditions.
	if (!rex.reg_ic && !has_mbyte)
	    while ((s = vim_strbyte(s, c)) != NULL)
	    {
		if (cstrncmp(s, prog->regmust, &prog->regmlen) == 0)
		    break;		// Found it.
		++s;
	    }
	else if (!rex.reg_ic || (!enc_utf8 && mb_char2len(c) > 1))
	    while ((s = vim_strchr(s, c)) != NULL)
	    {
		if (cstrncmp(s, prog->regmust, &prog->regmlen) == 0)
		    break;		// Found it.
		MB_PTR_ADV(s);
	    }
	else
	    while ((s = cstrchr(s, c)) != NULL)
	    {
		if (cstrncmp(s, prog->regmust, &prog->regmlen) == 0)
		    break;		// Found it.
		MB_PTR_ADV(s);
	    }
	if (s == NULL)		// Not present.
	    goto theend;
    }

    rex.line = line;
    rex.lnum = 0;
    reg_toolong = FALSE;

    // Simplest case: Anchored match need be tried only once.
    if (prog->reganch)
    {
	int	c;

	if (has_mbyte)
	    c = (*mb_ptr2char)(rex.line + col);
	else
	    c = rex.line[col];
	if (prog->regstart == NUL
		|| prog->regstart == c
		|| (rex.reg_ic
		    && (((enc_utf8 && utf_fold(prog->regstart) == utf_fold(c)))
			|| (c < 255 && prog->regstart < 255 &&
			    MB_TOLOWER(prog->regstart) == MB_TOLOWER(c)))))
	    retval = regtry(prog, col, tm, timed_out);
	else
	    retval = 0;
    }
    else
    {
#ifdef FEAT_RELTIME
	int tm_count = 0;
#endif
	// Messy cases:  unanchored match.
	while (!got_int)
	{
	    if (prog->regstart != NUL)
	    {
		// Skip until the char we know it must start with.
		// Used often, do some work to avoid call overhead.
		if (!rex.reg_ic && !has_mbyte)
		    s = vim_strbyte(rex.line + col, prog->regstart);
		else
		    s = cstrchr(rex.line + col, prog->regstart);
		if (s == NULL)
		{
		    retval = 0;
		    break;
		}
		col = (int)(s - rex.line);
	    }

	    // Check for maximum column to try.
	    if (rex.reg_maxcol > 0 && col >= rex.reg_maxcol)
	    {
		retval = 0;
		break;
	    }

	    retval = regtry(prog, col, tm, timed_out);
	    if (retval > 0)
		break;

	    // if not currently on the first line, get it again
	    if (rex.lnum != 0)
	    {
		rex.lnum = 0;
		rex.line = reg_getline((linenr_T)0);
	    }
	    if (rex.line[col] == NUL)
		break;
	    if (has_mbyte)
		col += (*mb_ptr2len)(rex.line + col);
	    else
		++col;
#ifdef FEAT_RELTIME
	    // Check for timeout once in a twenty times to avoid overhead.
	    if (tm != NULL && ++tm_count == 20)
	    {
		tm_count = 0;
		if (profile_passed_limit(tm))
		{
		    if (timed_out != NULL)
			*timed_out = TRUE;
		    break;
		}
	    }
#endif
	}
    }

theend:
    // Free "reg_tofree" when it's a bit big.
    // Free regstack and backpos if they are bigger than their initial size.
    if (reg_tofreelen > 400)
	VIM_CLEAR(reg_tofree);
    if (regstack.ga_maxlen > REGSTACK_INITIAL)
	ga_clear(&regstack);
    if (backpos.ga_maxlen > BACKPOS_INITIAL)
	ga_clear(&backpos);

    if (retval > 0)
    {
	// Make sure the end is never before the start.  Can happen when \zs
	// and \ze are used.
	if (REG_MULTI)
	{
	    lpos_T *start = &rex.reg_mmatch->startpos[0];
	    lpos_T *end = &rex.reg_mmatch->endpos[0];

	    if (end->lnum < start->lnum
			|| (end->lnum == start->lnum && end->col < start->col))
		rex.reg_mmatch->endpos[0] = rex.reg_mmatch->startpos[0];
	}
	else
	{
	    if (rex.reg_match->endp[0] < rex.reg_match->startp[0])
		rex.reg_match->endp[0] = rex.reg_match->startp[0];
	}
    }

    return retval;
}