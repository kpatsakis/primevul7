ex_spellrepall(exarg_T *eap UNUSED)
{
    pos_T	pos = curwin->w_cursor;
    char_u	*frompat;
    int		addlen;
    char_u	*line;
    char_u	*p;
    int		save_ws = p_ws;
    linenr_T	prev_lnum = 0;

    if (repl_from == NULL || repl_to == NULL)
    {
	emsg(_(e_no_previous_spell_replacement));
	return;
    }
    addlen = (int)(STRLEN(repl_to) - STRLEN(repl_from));

    frompat = alloc(STRLEN(repl_from) + 7);
    if (frompat == NULL)
	return;
    sprintf((char *)frompat, "\\V\\<%s\\>", repl_from);
    p_ws = FALSE;

    sub_nsubs = 0;
    sub_nlines = 0;
    curwin->w_cursor.lnum = 0;
    while (!got_int)
    {
	if (do_search(NULL, '/', '/', frompat, 1L, SEARCH_KEEP, NULL) == 0
						   || u_save_cursor() == FAIL)
	    break;

	// Only replace when the right word isn't there yet.  This happens
	// when changing "etc" to "etc.".
	line = ml_get_curline();
	if (addlen <= 0 || STRNCMP(line + curwin->w_cursor.col,
					       repl_to, STRLEN(repl_to)) != 0)
	{
	    p = alloc(STRLEN(line) + addlen + 1);
	    if (p == NULL)
		break;
	    mch_memmove(p, line, curwin->w_cursor.col);
	    STRCPY(p + curwin->w_cursor.col, repl_to);
	    STRCAT(p, line + curwin->w_cursor.col + STRLEN(repl_from));
	    ml_replace(curwin->w_cursor.lnum, p, FALSE);
	    changed_bytes(curwin->w_cursor.lnum, curwin->w_cursor.col);
	    if (curbuf->b_has_textprop && addlen != 0)
		adjust_prop_columns(curwin->w_cursor.lnum,
				 curwin->w_cursor.col, addlen, APC_SUBSTITUTE);

	    if (curwin->w_cursor.lnum != prev_lnum)
	    {
		++sub_nlines;
		prev_lnum = curwin->w_cursor.lnum;
	    }
	    ++sub_nsubs;
	}
	curwin->w_cursor.col += (colnr_T)STRLEN(repl_to);
    }

    p_ws = save_ws;
    curwin->w_cursor = pos;
    vim_free(frompat);

    if (sub_nsubs == 0)
	semsg(_(e_not_found_str), repl_from);
    else
	do_sub_msg(FALSE);
}