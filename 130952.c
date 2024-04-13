ex_append(exarg_T *eap)
{
    char_u	*theline;
    int		did_undo = FALSE;
    linenr_T	lnum = eap->line2;
    int		indent = 0;
    char_u	*p;
    int		vcol;
    int		empty = (curbuf->b_ml.ml_flags & ML_EMPTY);

#ifdef FEAT_EVAL
    if (not_in_vim9(eap) == FAIL)
	return;
#endif
    // the ! flag toggles autoindent
    if (eap->forceit)
	curbuf->b_p_ai = !curbuf->b_p_ai;

    // First autoindent comes from the line we start on
    if (eap->cmdidx != CMD_change && curbuf->b_p_ai && lnum > 0)
	append_indent = get_indent_lnum(lnum);

    if (eap->cmdidx != CMD_append)
	--lnum;

    // when the buffer is empty need to delete the dummy line
    if (empty && lnum == 1)
	lnum = 0;

    State = MODE_INSERT;		    // behave like in Insert mode
    if (curbuf->b_p_iminsert == B_IMODE_LMAP)
	State |= MODE_LANGMAP;

    for (;;)
    {
	msg_scroll = TRUE;
	need_wait_return = FALSE;
	if (curbuf->b_p_ai)
	{
	    if (append_indent >= 0)
	    {
		indent = append_indent;
		append_indent = -1;
	    }
	    else if (lnum > 0)
		indent = get_indent_lnum(lnum);
	}
	ex_keep_indent = FALSE;
	if (eap->getline == NULL)
	{
	    // No getline() function, use the lines that follow. This ends
	    // when there is no more.
	    if (eap->nextcmd == NULL || *eap->nextcmd == NUL)
		break;
	    p = vim_strchr(eap->nextcmd, NL);
	    if (p == NULL)
		p = eap->nextcmd + STRLEN(eap->nextcmd);
	    theline = vim_strnsave(eap->nextcmd, p - eap->nextcmd);
	    if (*p != NUL)
		++p;
	    eap->nextcmd = p;
	}
	else
	{
	    int save_State = State;

	    // Set State to avoid the cursor shape to be set to MODE_INSERT
	    // state when getline() returns.
	    State = MODE_CMDLINE;
	    theline = eap->getline(
#ifdef FEAT_EVAL
		    eap->cstack->cs_looplevel > 0 ? -1 :
#endif
		    NUL, eap->cookie, indent, TRUE);
	    State = save_State;
	}
	lines_left = Rows - 1;
	if (theline == NULL)
	    break;

	// Using ^ CTRL-D in getexmodeline() makes us repeat the indent.
	if (ex_keep_indent)
	    append_indent = indent;

	// Look for the "." after automatic indent.
	vcol = 0;
	for (p = theline; indent > vcol; ++p)
	{
	    if (*p == ' ')
		++vcol;
	    else if (*p == TAB)
		vcol += 8 - vcol % 8;
	    else
		break;
	}
	if ((p[0] == '.' && p[1] == NUL)
		|| (!did_undo && u_save(lnum, lnum + 1 + (empty ? 1 : 0))
								     == FAIL))
	{
	    vim_free(theline);
	    break;
	}

	// don't use autoindent if nothing was typed.
	if (p[0] == NUL)
	    theline[0] = NUL;

	did_undo = TRUE;
	ml_append(lnum, theline, (colnr_T)0, FALSE);
	if (empty)
	    // there are no marks below the inserted lines
	    appended_lines(lnum, 1L);
	else
	    appended_lines_mark(lnum, 1L);

	vim_free(theline);
	++lnum;

	if (empty)
	{
	    ml_delete(2L);
	    empty = FALSE;
	}
    }
    State = MODE_NORMAL;

    if (eap->forceit)
	curbuf->b_p_ai = !curbuf->b_p_ai;

    // "start" is set to eap->line2+1 unless that position is invalid (when
    // eap->line2 pointed to the end of the buffer and nothing was appended)
    // "end" is set to lnum when something has been appended, otherwise
    // it is the same as "start"  -- Acevedo
    if ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	curbuf->b_op_start.lnum = (eap->line2 < curbuf->b_ml.ml_line_count) ?
	    eap->line2 + 1 : curbuf->b_ml.ml_line_count;
	if (eap->cmdidx != CMD_append)
	    --curbuf->b_op_start.lnum;
	curbuf->b_op_end.lnum = (eap->line2 < lnum)
					      ? lnum : curbuf->b_op_start.lnum;
	curbuf->b_op_start.col = curbuf->b_op_end.col = 0;
    }
    curwin->w_cursor.lnum = lnum;
    check_cursor_lnum();
    beginline(BL_SOL | BL_FIX);

    need_wait_return = FALSE;	// don't use wait_return() now
    ex_no_reprint = TRUE;
}