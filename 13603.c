nfa_regpiece(void)
{
    int		i;
    int		op;
    int		ret;
    long	minval, maxval;
    int		greedy = TRUE;      // Braces are prefixed with '-' ?
    parse_state_T old_state;
    parse_state_T new_state;
    long	c2;
    int		old_post_pos;
    int		my_post_start;
    int		quest;

    // Save the current parse state, so that we can use it if <atom>{m,n} is
    // next.
    save_parse_state(&old_state);

    // store current pos in the postfix form, for \{m,n} involving 0s
    my_post_start = (int)(post_ptr - post_start);

    ret = nfa_regatom();
    if (ret == FAIL)
	return FAIL;	    // cascaded error

    op = peekchr();
    if (re_multi_type(op) == NOT_MULTI)
	return OK;

    skipchr();
    switch (op)
    {
	case Magic('*'):
	    EMIT(NFA_STAR);
	    break;

	case Magic('+'):
	    /*
	     * Trick: Normally, (a*)\+ would match the whole input "aaa".  The
	     * first and only submatch would be "aaa". But the backtracking
	     * engine interprets the plus as "try matching one more time", and
	     * a* matches a second time at the end of the input, the empty
	     * string.
	     * The submatch will be the empty string.
	     *
	     * In order to be consistent with the old engine, we replace
	     * <atom>+ with <atom><atom>*
	     */
	    restore_parse_state(&old_state);
	    curchr = -1;
	    if (nfa_regatom() == FAIL)
		return FAIL;
	    EMIT(NFA_STAR);
	    EMIT(NFA_CONCAT);
	    skipchr();		// skip the \+
	    break;

	case Magic('@'):
	    c2 = getdecchrs();
	    op = no_Magic(getchr());
	    i = 0;
	    switch(op)
	    {
		case '=':
		    // \@=
		    i = NFA_PREV_ATOM_NO_WIDTH;
		    break;
		case '!':
		    // \@!
		    i = NFA_PREV_ATOM_NO_WIDTH_NEG;
		    break;
		case '<':
		    op = no_Magic(getchr());
		    if (op == '=')
			// \@<=
			i = NFA_PREV_ATOM_JUST_BEFORE;
		    else if (op == '!')
			// \@<!
			i = NFA_PREV_ATOM_JUST_BEFORE_NEG;
		    break;
		case '>':
		    // \@>
		    i = NFA_PREV_ATOM_LIKE_PATTERN;
		    break;
	    }
	    if (i == 0)
	    {
		semsg(_("E869: (NFA) Unknown operator '\\@%c'"), op);
		return FAIL;
	    }
	    EMIT(i);
	    if (i == NFA_PREV_ATOM_JUST_BEFORE
					|| i == NFA_PREV_ATOM_JUST_BEFORE_NEG)
		EMIT(c2);
	    break;

	case Magic('?'):
	case Magic('='):
	    EMIT(NFA_QUEST);
	    break;

	case Magic('{'):
	    // a{2,5} will expand to 'aaa?a?a?'
	    // a{-1,3} will expand to 'aa??a??', where ?? is the nongreedy
	    // version of '?'
	    // \v(ab){2,3} will expand to '(ab)(ab)(ab)?', where all the
	    // parenthesis have the same id

	    greedy = TRUE;
	    c2 = peekchr();
	    if (c2 == '-' || c2 == Magic('-'))
	    {
		skipchr();
		greedy = FALSE;
	    }
	    if (!read_limits(&minval, &maxval))
		EMSG_RET_FAIL(_("E870: (NFA regexp) Error reading repetition limits"));

	    //  <atom>{0,inf}, <atom>{0,} and <atom>{}  are equivalent to
	    //  <atom>*
	    if (minval == 0 && maxval == MAX_LIMIT)
	    {
		if (greedy)		// { { (match the braces)
		    // \{}, \{0,}
		    EMIT(NFA_STAR);
		else			// { { (match the braces)
		    // \{-}, \{-0,}
		    EMIT(NFA_STAR_NONGREEDY);
		break;
	    }

	    // Special case: x{0} or x{-0}
	    if (maxval == 0)
	    {
		// Ignore result of previous call to nfa_regatom()
		post_ptr = post_start + my_post_start;
		// NFA_EMPTY is 0-length and works everywhere
		EMIT(NFA_EMPTY);
		return OK;
	    }

	    // The engine is very inefficient (uses too many states) when the
	    // maximum is much larger than the minimum and when the maximum is
	    // large.  However, when maxval is MAX_LIMIT, it is okay, as this
	    // will emit NFA_STAR.
	    // Bail out if we can use the other engine, but only, when the
	    // pattern does not need the NFA engine like (e.g. [[:upper:]]\{2,\}
	    // does not work with with characters > 8 bit with the BT engine)
	    if ((nfa_re_flags & RE_AUTO)
				   && (maxval > 500 || maxval > minval + 200)
				   && (maxval != MAX_LIMIT && minval < 200)
				   && !wants_nfa)
		return FAIL;

	    // Ignore previous call to nfa_regatom()
	    post_ptr = post_start + my_post_start;
	    // Save parse state after the repeated atom and the \{}
	    save_parse_state(&new_state);

	    quest = (greedy == TRUE? NFA_QUEST : NFA_QUEST_NONGREEDY);
	    for (i = 0; i < maxval; i++)
	    {
		// Goto beginning of the repeated atom
		restore_parse_state(&old_state);
		old_post_pos = (int)(post_ptr - post_start);
		if (nfa_regatom() == FAIL)
		    return FAIL;
		// after "minval" times, atoms are optional
		if (i + 1 > minval)
		{
		    if (maxval == MAX_LIMIT)
		    {
			if (greedy)
			    EMIT(NFA_STAR);
			else
			    EMIT(NFA_STAR_NONGREEDY);
		    }
		    else
			EMIT(quest);
		}
		if (old_post_pos != my_post_start)
		    EMIT(NFA_CONCAT);
		if (i + 1 > minval && maxval == MAX_LIMIT)
		    break;
	    }

	    // Go to just after the repeated atom and the \{}
	    restore_parse_state(&new_state);
	    curchr = -1;

	    break;


	default:
	    break;
    }	// end switch

    if (re_multi_type(peekchr()) != NOT_MULTI)
	// Can't have a multi follow a multi.
	EMSG_RET_FAIL(_("E871: (NFA regexp) Can't have a multi follow a multi"));

    return OK;
}