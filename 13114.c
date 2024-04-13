regtry(
    bt_regprog_T	*prog,
    colnr_T		col,
    proftime_T		*tm,		// timeout limit or NULL
    int			*timed_out)	// flag set on timeout or NULL
{
    rex.input = rex.line + col;
    rex.need_clear_subexpr = TRUE;
#ifdef FEAT_SYN_HL
    // Clear the external match subpointers if necessary.
    rex.need_clear_zsubexpr = (prog->reghasz == REX_SET);
#endif

    if (regmatch(prog->program + 1, tm, timed_out) == 0)
	return 0;

    cleanup_subexpr();
    if (REG_MULTI)
    {
	if (rex.reg_startpos[0].lnum < 0)
	{
	    rex.reg_startpos[0].lnum = 0;
	    rex.reg_startpos[0].col = col;
	}
	if (rex.reg_endpos[0].lnum < 0)
	{
	    rex.reg_endpos[0].lnum = rex.lnum;
	    rex.reg_endpos[0].col = (int)(rex.input - rex.line);
	}
	else
	    // Use line number of "\ze".
	    rex.lnum = rex.reg_endpos[0].lnum;
    }
    else
    {
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
	int		i;

	cleanup_zsubexpr();
	re_extmatch_out = make_extmatch();
	if (re_extmatch_out == NULL)
	    return 0;
	for (i = 0; i < NSUBEXP; i++)
	{
	    if (REG_MULTI)
	    {
		// Only accept single line matches.
		if (reg_startzpos[i].lnum >= 0
			&& reg_endzpos[i].lnum == reg_startzpos[i].lnum
			&& reg_endzpos[i].col >= reg_startzpos[i].col)
		    re_extmatch_out->matches[i] =
			vim_strnsave(reg_getline(reg_startzpos[i].lnum)
						       + reg_startzpos[i].col,
				   reg_endzpos[i].col - reg_startzpos[i].col);
	    }
	    else
	    {
		if (reg_startzp[i] != NULL && reg_endzp[i] != NULL)
		    re_extmatch_out->matches[i] =
			    vim_strnsave(reg_startzp[i],
						reg_endzp[i] - reg_startzp[i]);
	    }
	}
    }
#endif
    return 1 + rex.lnum;
}