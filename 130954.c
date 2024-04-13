ex_global(exarg_T *eap)
{
    linenr_T	lnum;		// line number according to old situation
    int		ndone = 0;
    int		type;		// first char of cmd: 'v' or 'g'
    char_u	*cmd;		// command argument

    char_u	delim;		// delimiter, normally '/'
    char_u	*pat;
    regmmatch_T	regmatch;
    int		match;
    int		which_pat;

    // When nesting the command works on one line.  This allows for
    // ":g/found/v/notfound/command".
    if (global_busy && (eap->line1 != 1
				  || eap->line2 != curbuf->b_ml.ml_line_count))
    {
	// will increment global_busy to break out of the loop
	emsg(_(e_cannot_do_global_recursive_with_range));
	return;
    }

    if (eap->forceit)		    // ":global!" is like ":vglobal"
	type = 'v';
    else
	type = *eap->cmd;
    cmd = eap->arg;
    which_pat = RE_LAST;	    // default: use last used regexp

#ifdef FEAT_EVAL
    if (in_vim9script() && check_global_and_subst(eap->cmd, eap->arg) == FAIL)
	return;
#endif

    /*
     * undocumented vi feature:
     *	"\/" and "\?": use previous search pattern.
     *		 "\&": use previous substitute pattern.
     */
    if (*cmd == '\\')
    {
	++cmd;
	if (vim_strchr((char_u *)"/?&", *cmd) == NULL)
	{
	    emsg(_(e_backslash_should_be_followed_by));
	    return;
	}
	if (*cmd == '&')
	    which_pat = RE_SUBST;	// use previous substitute pattern
	else
	    which_pat = RE_SEARCH;	// use previous search pattern
	++cmd;
	pat = (char_u *)"";
    }
    else if (*cmd == NUL)
    {
	emsg(_(e_regular_expression_missing_from_global));
	return;
    }
    else if (check_regexp_delim(*cmd) == FAIL)
    {
	return;
    }
    else
    {
	delim = *cmd;		// get the delimiter
	++cmd;			// skip delimiter if there is one
	pat = cmd;		// remember start of pattern
	cmd = skip_regexp_ex(cmd, delim, magic_isset(), &eap->arg, NULL, NULL);
	if (cmd[0] == delim)		    // end delimiter found
	    *cmd++ = NUL;		    // replace it with a NUL
    }

    if (search_regcomp(pat, RE_BOTH, which_pat, SEARCH_HIS, &regmatch) == FAIL)
    {
	emsg(_(e_invalid_command));
	return;
    }

    if (global_busy)
    {
	lnum = curwin->w_cursor.lnum;
	match = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, NULL, NULL);
	if ((type == 'g' && match) || (type == 'v' && !match))
	    global_exe_one(cmd, lnum);
    }
    else
    {
	/*
	 * pass 1: set marks for each (not) matching line
	 */
	for (lnum = eap->line1; lnum <= eap->line2 && !got_int; ++lnum)
	{
	    // a match on this line?
	    match = vim_regexec_multi(&regmatch, curwin, curbuf, lnum,
						       (colnr_T)0, NULL, NULL);
	    if (regmatch.regprog == NULL)
		break;  // re-compiling regprog failed
	    if ((type == 'g' && match) || (type == 'v' && !match))
	    {
		ml_setmarked(lnum);
		ndone++;
	    }
	    line_breakcheck();
	}

	/*
	 * pass 2: execute the command for each line that has been marked
	 */
	if (got_int)
	    msg(_(e_interrupted));
	else if (ndone == 0)
	{
	    if (type == 'v')
	    {
		if (in_vim9script())
		    semsg(_(e_pattern_found_in_every_line_str), pat);
		else
		    smsg(_("Pattern found in every line: %s"), pat);
	    }
	    else
	    {
		if (in_vim9script())
		    semsg(_(e_pattern_not_found_str), pat);
		else
		    smsg(_("Pattern not found: %s"), pat);
	    }
	}
	else
	{
#ifdef FEAT_CLIPBOARD
	    start_global_changes();
#endif
	    global_exe(cmd);
#ifdef FEAT_CLIPBOARD
	    end_global_changes();
#endif
	}

	ml_clearmarked();	   // clear rest of the marks
    }

    vim_regfree(regmatch.regprog);
}