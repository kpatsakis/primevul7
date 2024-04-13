recursive_regmatch(
    nfa_state_T	    *state,
    nfa_pim_T	    *pim,
    nfa_regprog_T   *prog,
    regsubs_T	    *submatch,
    regsubs_T	    *m,
    int		    **listids,
    int		    *listids_len)
{
    int		save_reginput_col = (int)(rex.input - rex.line);
    int		save_reglnum = rex.lnum;
    int		save_nfa_match = nfa_match;
    int		save_nfa_listid = rex.nfa_listid;
    save_se_T   *save_nfa_endp = nfa_endp;
    save_se_T   endpos;
    save_se_T   *endposp = NULL;
    int		result;
    int		need_restore = FALSE;

    if (pim != NULL)
    {
	// start at the position where the postponed match was
	if (REG_MULTI)
	    rex.input = rex.line + pim->end.pos.col;
	else
	    rex.input = pim->end.ptr;
    }

    if (state->c == NFA_START_INVISIBLE_BEFORE
	    || state->c == NFA_START_INVISIBLE_BEFORE_FIRST
	    || state->c == NFA_START_INVISIBLE_BEFORE_NEG
	    || state->c == NFA_START_INVISIBLE_BEFORE_NEG_FIRST)
    {
	// The recursive match must end at the current position. When "pim" is
	// not NULL it specifies the current position.
	endposp = &endpos;
	if (REG_MULTI)
	{
	    if (pim == NULL)
	    {
		endpos.se_u.pos.col = (int)(rex.input - rex.line);
		endpos.se_u.pos.lnum = rex.lnum;
	    }
	    else
		endpos.se_u.pos = pim->end.pos;
	}
	else
	{
	    if (pim == NULL)
		endpos.se_u.ptr = rex.input;
	    else
		endpos.se_u.ptr = pim->end.ptr;
	}

	// Go back the specified number of bytes, or as far as the
	// start of the previous line, to try matching "\@<=" or
	// not matching "\@<!". This is very inefficient, limit the number of
	// bytes if possible.
	if (state->val <= 0)
	{
	    if (REG_MULTI)
	    {
		rex.line = reg_getline(--rex.lnum);
		if (rex.line == NULL)
		    // can't go before the first line
		    rex.line = reg_getline(++rex.lnum);
	    }
	    rex.input = rex.line;
	}
	else
	{
	    if (REG_MULTI && (int)(rex.input - rex.line) < state->val)
	    {
		// Not enough bytes in this line, go to end of
		// previous line.
		rex.line = reg_getline(--rex.lnum);
		if (rex.line == NULL)
		{
		    // can't go before the first line
		    rex.line = reg_getline(++rex.lnum);
		    rex.input = rex.line;
		}
		else
		    rex.input = rex.line + STRLEN(rex.line);
	    }
	    if ((int)(rex.input - rex.line) >= state->val)
	    {
		rex.input -= state->val;
		if (has_mbyte)
		    rex.input -= mb_head_off(rex.line, rex.input);
	    }
	    else
		rex.input = rex.line;
	}
    }

#ifdef ENABLE_LOG
    if (log_fd != stderr)
	fclose(log_fd);
    log_fd = NULL;
#endif
    // Have to clear the lastlist field of the NFA nodes, so that
    // nfa_regmatch() and addstate() can run properly after recursion.
    if (nfa_ll_index == 1)
    {
	// Already calling nfa_regmatch() recursively.  Save the lastlist[1]
	// values and clear them.
	if (*listids == NULL || *listids_len < prog->nstate)
	{
	    vim_free(*listids);
	    *listids = ALLOC_MULT(int, prog->nstate);
	    if (*listids == NULL)
	    {
		emsg(_("E878: (NFA) Could not allocate memory for branch traversal!"));
		return 0;
	    }
	    *listids_len = prog->nstate;
	}
	nfa_save_listids(prog, *listids);
	need_restore = TRUE;
	// any value of rex.nfa_listid will do
    }
    else
    {
	// First recursive nfa_regmatch() call, switch to the second lastlist
	// entry.  Make sure rex.nfa_listid is different from a previous
	// recursive call, because some states may still have this ID.
	++nfa_ll_index;
	if (rex.nfa_listid <= rex.nfa_alt_listid)
	    rex.nfa_listid = rex.nfa_alt_listid;
    }

    // Call nfa_regmatch() to check if the current concat matches at this
    // position. The concat ends with the node NFA_END_INVISIBLE
    nfa_endp = endposp;
    result = nfa_regmatch(prog, state->out, submatch, m);

    if (need_restore)
	nfa_restore_listids(prog, *listids);
    else
    {
	--nfa_ll_index;
	rex.nfa_alt_listid = rex.nfa_listid;
    }

    // restore position in input text
    rex.lnum = save_reglnum;
    if (REG_MULTI)
	rex.line = reg_getline(rex.lnum);
    rex.input = rex.line + save_reginput_col;
    if (result != NFA_TOO_EXPENSIVE)
    {
	nfa_match = save_nfa_match;
	rex.nfa_listid = save_nfa_listid;
    }
    nfa_endp = save_nfa_endp;

#ifdef ENABLE_LOG
    log_fd = fopen(NFA_REGEXP_RUN_LOG, "a");
    if (log_fd != NULL)
    {
	fprintf(log_fd, "****************************\n");
	fprintf(log_fd, "FINISHED RUNNING nfa_regmatch() recursively\n");
	fprintf(log_fd, "MATCH = %s\n", result == TRUE ? "OK" : "FALSE");
	fprintf(log_fd, "****************************\n");
    }
    else
    {
	emsg(_(e_log_open_failed));
	log_fd = stderr;
    }
#endif

    return result;
}