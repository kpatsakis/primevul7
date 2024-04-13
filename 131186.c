check_need_cap(linenr_T lnum, colnr_T col)
{
    int		need_cap = FALSE;
    char_u	*line;
    char_u	*line_copy = NULL;
    char_u	*p;
    colnr_T	endcol;
    regmatch_T	regmatch;

    if (curwin->w_s->b_cap_prog == NULL)
	return FALSE;

    line = ml_get_curline();
    endcol = 0;
    if (getwhitecols(line) >= (int)col)
    {
	// At start of line, check if previous line is empty or sentence
	// ends there.
	if (lnum == 1)
	    need_cap = TRUE;
	else
	{
	    line = ml_get(lnum - 1);
	    if (*skipwhite(line) == NUL)
		need_cap = TRUE;
	    else
	    {
		// Append a space in place of the line break.
		line_copy = concat_str(line, (char_u *)" ");
		line = line_copy;
		endcol = (colnr_T)STRLEN(line);
	    }
	}
    }
    else
	endcol = col;

    if (endcol > 0)
    {
	// Check if sentence ends before the bad word.
	regmatch.regprog = curwin->w_s->b_cap_prog;
	regmatch.rm_ic = FALSE;
	p = line + endcol;
	for (;;)
	{
	    MB_PTR_BACK(line, p);
	    if (p == line || spell_iswordp_nmw(p, curwin))
		break;
	    if (vim_regexec(&regmatch, p, 0)
					 && regmatch.endp[0] == line + endcol)
	    {
		need_cap = TRUE;
		break;
	    }
	}
	curwin->w_s->b_cap_prog = regmatch.regprog;
    }

    vim_free(line_copy);

    return need_cap;
}