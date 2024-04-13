nfa_regtry(
    nfa_regprog_T   *prog,
    colnr_T	    col,
    proftime_T	    *tm UNUSED,	// timeout limit or NULL
    int		    *timed_out UNUSED)	// flag set on timeout or NULL
{
    int		i;
    regsubs_T	subs, m;
    nfa_state_T	*start = prog->start;
    int		result;
#ifdef ENABLE_LOG
    FILE	*f;
#endif

    rex.input = rex.line + col;
#ifdef FEAT_RELTIME
    nfa_time_limit = tm;
    nfa_timed_out = timed_out;
    nfa_time_count = 0;
#endif

#ifdef ENABLE_LOG
    f = fopen(NFA_REGEXP_RUN_LOG, "a");
    if (f != NULL)
    {
	fprintf(f, "\n\n\t=======================================================\n");
#ifdef DEBUG
	fprintf(f, "\tRegexp is \"%s\"\n", nfa_regengine.expr);
#endif
	fprintf(f, "\tInput text is \"%s\" \n", rex.input);
	fprintf(f, "\t=======================================================\n\n");
	nfa_print_state(f, start);
	fprintf(f, "\n\n");
	fclose(f);
    }
    else
	emsg("Could not open temporary log file for writing");
#endif

    clear_sub(&subs.norm);
    clear_sub(&m.norm);
#ifdef FEAT_SYN_HL
    clear_sub(&subs.synt);
    clear_sub(&m.synt);
#endif

    result = nfa_regmatch(prog, start, &subs, &m);
    if (result == FALSE)
	return 0;
    else if (result == NFA_TOO_EXPENSIVE)
	return result;

    cleanup_subexpr();
    if (REG_MULTI)
    {
	for (i = 0; i < subs.norm.in_use; i++)
	{
	    rex.reg_startpos[i].lnum = subs.norm.list.multi[i].start_lnum;
	    rex.reg_startpos[i].col = subs.norm.list.multi[i].start_col;

	    rex.reg_endpos[i].lnum = subs.norm.list.multi[i].end_lnum;
	    rex.reg_endpos[i].col = subs.norm.list.multi[i].end_col;
	}

	if (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].col = col;
	}
	if (rex.reg_endpos[0].lnum < 0)
	{
	    // pattern has a \ze but it didn't match, use current end
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].col = (int)(rex.input - rex.line);
	}
	else
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    else
    {
	for (i = 0; i < subs.norm.in_use; i++)
	{
	    rex.reg_startp[i] = subs.norm.list.line[i].start;
	    rex.reg_endp[i] = subs.norm.list.line[i].end;
	}

	if (rex.reg_startp[0] == NULL)
	    rex.reg_startp[0] = rex.line + col;
	if (rex.reg_endp[0] == NULL)
	    rex.reg_endp[0] = rex.input;
    }

#ifdef FEAT_SYN_HL
    // Package any found \z(...\) matches for export. Default is none.
    unref_extmatch(re_extmatch_out);
    re_extmatch_out = NULL;

    if (prog->reghasz == REX_SET)
    {
	cleanup_zsubexpr();
	re_extmatch_out = make_extmatch();
	if (re_extmatch_out == NULL)
	    return 0;
	// Loop over \z1, \z2, etc.  There is no \z0.
	for (i = 1; i < subs.synt.in_use; i++)
	{
	    if (REG_MULTI)
	    {
		struct multipos *mpos = &subs.synt.list.multi[i];

		// Only accept single line matches that are valid.
		if (mpos->start_lnum >= 0
			&& mpos->start_lnum == mpos->end_lnum
			&& mpos->end_col >= mpos->start_col)
		    re_extmatch_out->matches[i] =
			vim_strnsave(reg_getline(mpos->start_lnum)
							    + mpos->start_col,
					     mpos->end_col - mpos->start_col);
	    }
	    else
	    {
		struct linepos *lpos = &subs.synt.list.line[i];

		if (lpos->start != NULL && lpos->end != NULL)
		    re_extmatch_out->matches[i] =
			    vim_strnsave(lpos->start, lpos->end - lpos->start);
	    }
	}
    }
#endif

    return 1 + rex.lnum;
}