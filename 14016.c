build_stl_str_hl(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	*fmt,
    int		use_sandbox UNUSED, // "fmt" was set insecurely, use sandbox
    int		fillchar,
    int		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab)	// return: tab page nrs (can be NULL)
{
    linenr_T	lnum;
    size_t	len;
    char_u	*p;
    char_u	*s;
    char_u	*t;
    int		byteval;
#ifdef FEAT_EVAL
    win_T	*save_curwin;
    buf_T	*save_curbuf;
    int		save_VIsual_active;
#endif
    int		empty_line;
    colnr_T	virtcol;
    long	l;
    long	n;
    int		prevchar_isflag;
    int		prevchar_isitem;
    int		itemisflag;
    int		fillable;
    char_u	*str;
    long	num;
    int		width;
    int		itemcnt;
    int		curitem;
    int		group_end_userhl;
    int		group_start_userhl;
    int		groupdepth;
#ifdef FEAT_EVAL
    int		evaldepth;
#endif
    int		minwid;
    int		maxwid;
    int		zeropad;
    char_u	base;
    char_u	opt;
#define TMPLEN 70
    char_u	buf_tmp[TMPLEN];
    char_u	win_tmp[TMPLEN];
    char_u	*usefmt = fmt;
    stl_hlrec_T *sp;
    int		save_must_redraw = must_redraw;
    int		save_redr_type = curwin->w_redr_type;
    int		save_KeyTyped = KeyTyped;

    if (stl_items == NULL)
    {
	stl_items = ALLOC_MULT(stl_item_T, stl_items_len);
	stl_groupitem = ALLOC_MULT(int, stl_items_len);

	// Allocate one more, because the last element is used to indicate the
	// end of the list.
	stl_hltab  = ALLOC_MULT(stl_hlrec_T, stl_items_len + 1);
	stl_tabtab = ALLOC_MULT(stl_hlrec_T, stl_items_len + 1);
    }

#ifdef FEAT_EVAL
    /*
     * When the format starts with "%!" then evaluate it as an expression and
     * use the result as the actual format string.
     */
    if (fmt[0] == '%' && fmt[1] == '!')
    {
	typval_T	tv;

	tv.v_type = VAR_NUMBER;
	tv.vval.v_number = wp->w_id;
	set_var((char_u *)"g:statusline_winid", &tv, FALSE);

	usefmt = eval_to_string_safe(fmt + 2, use_sandbox, FALSE);
	if (usefmt == NULL)
	    usefmt = fmt;

	do_unlet((char_u *)"g:statusline_winid", TRUE);
    }
#endif

    if (fillchar == 0)
	fillchar = ' ';

    // The cursor in windows other than the current one isn't always
    // up-to-date, esp. because of autocommands and timers.
    lnum = wp->w_cursor.lnum;
    if (lnum > wp->w_buffer->b_ml.ml_line_count)
    {
	lnum = wp->w_buffer->b_ml.ml_line_count;
	wp->w_cursor.lnum = lnum;
    }

    // Get line & check if empty (cursorpos will show "0-1").  Note that
    // p will become invalid when getting another buffer line.
    p = ml_get_buf(wp->w_buffer, lnum, FALSE);
    empty_line = (*p == NUL);

    // Get the byte value now, in case we need it below. This is more efficient
    // than making a copy of the line.
    len = STRLEN(p);
    if (wp->w_cursor.col > (colnr_T)len)
    {
	// Line may have changed since checking the cursor column, or the lnum
	// was adjusted above.
	wp->w_cursor.col = (colnr_T)len;
	wp->w_cursor.coladd = 0;
	byteval = 0;
    }
    else
	byteval = (*mb_ptr2char)(p + wp->w_cursor.col);

    groupdepth = 0;
#ifdef FEAT_EVAL
    evaldepth = 0;
#endif
    p = out;
    curitem = 0;
    prevchar_isflag = TRUE;
    prevchar_isitem = FALSE;
    for (s = usefmt; *s; )
    {
	if (curitem == (int)stl_items_len)
	{
	    size_t	new_len = stl_items_len * 3 / 2;
	    stl_item_T	*new_items;
	    int		*new_groupitem;
	    stl_hlrec_T	*new_hlrec;

	    new_items = vim_realloc(stl_items, sizeof(stl_item_T) * new_len);
	    if (new_items == NULL)
		break;
	    stl_items = new_items;
	    new_groupitem = vim_realloc(stl_groupitem, sizeof(int) * new_len);
	    if (new_groupitem == NULL)
		break;
	    stl_groupitem = new_groupitem;
	    new_hlrec = vim_realloc(stl_hltab,
					  sizeof(stl_hlrec_T) * (new_len + 1));
	    if (new_hlrec == NULL)
		break;
	    stl_hltab = new_hlrec;
	    new_hlrec = vim_realloc(stl_tabtab,
					  sizeof(stl_hlrec_T) * (new_len + 1));
	    if (new_hlrec == NULL)
		break;
	    stl_tabtab = new_hlrec;
	    stl_items_len = new_len;
	}

	if (*s != NUL && *s != '%')
	    prevchar_isflag = prevchar_isitem = FALSE;

	/*
	 * Handle up to the next '%' or the end.
	 */
	while (*s != NUL && *s != '%' && p + 1 < out + outlen)
	    *p++ = *s++;
	if (*s == NUL || p + 1 >= out + outlen)
	    break;

	/*
	 * Handle one '%' item.
	 */
	s++;
	if (*s == NUL)  // ignore trailing %
	    break;
	if (*s == '%')
	{
	    if (p + 1 >= out + outlen)
		break;
	    *p++ = *s++;
	    prevchar_isflag = prevchar_isitem = FALSE;
	    continue;
	}
	if (*s == STL_MIDDLEMARK)
	{
	    s++;
	    if (groupdepth > 0)
		continue;
	    stl_items[curitem].stl_type = Middle;
	    stl_items[curitem++].stl_start = p;
	    continue;
	}
	if (*s == STL_TRUNCMARK)
	{
	    s++;
	    stl_items[curitem].stl_type = Trunc;
	    stl_items[curitem++].stl_start = p;
	    continue;
	}
	if (*s == ')')
	{
	    s++;
	    if (groupdepth < 1)
		continue;
	    groupdepth--;

	    t = stl_items[stl_groupitem[groupdepth]].stl_start;
	    *p = NUL;
	    l = vim_strsize(t);
	    if (curitem > stl_groupitem[groupdepth] + 1
		    && stl_items[stl_groupitem[groupdepth]].stl_minwid == 0)
	    {
		// remove group if all items are empty and highlight group
		// doesn't change
		group_start_userhl = group_end_userhl = 0;
		for (n = stl_groupitem[groupdepth] - 1; n >= 0; n--)
		{
		    if (stl_items[n].stl_type == Highlight)
		    {
			group_start_userhl = group_end_userhl =
						       stl_items[n].stl_minwid;
			break;
		    }
		}
		for (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		{
		    if (stl_items[n].stl_type == Normal)
			break;
		    if (stl_items[n].stl_type == Highlight)
			group_end_userhl = stl_items[n].stl_minwid;
		}
		if (n == curitem && group_start_userhl == group_end_userhl)
		{
		    // empty group
		    p = t;
		    l = 0;
		    for (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		    {
			// do not use the highlighting from the removed group
			if (stl_items[n].stl_type == Highlight)
			    stl_items[n].stl_type = Empty;
			// adjust the start position of TabPage to the next
			// item position
			if (stl_items[n].stl_type == TabPage)
			    stl_items[n].stl_start = p;
		    }
		}
	    }
	    if (l > stl_items[stl_groupitem[groupdepth]].stl_maxwid)
	    {
		// truncate, remove n bytes of text at the start
		if (has_mbyte)
		{
		    // Find the first character that should be included.
		    n = 0;
		    while (l >= stl_items[stl_groupitem[groupdepth]].stl_maxwid)
		    {
			l -= ptr2cells(t + n);
			n += (*mb_ptr2len)(t + n);
		    }
		}
		else
		    n = (long)(p - t) - stl_items[stl_groupitem[groupdepth]]
							       .stl_maxwid + 1;

		*t = '<';
		mch_memmove(t + 1, t + n, (size_t)(p - (t + n)));
		p = p - n + 1;

		// Fill up space left over by half a double-wide char.
		while (++l < stl_items[stl_groupitem[groupdepth]].stl_minwid)
		    MB_CHAR2BYTES(fillchar, p);

		// correct the start of the items for the truncation
		for (l = stl_groupitem[groupdepth] + 1; l < curitem; l++)
		{
		    stl_items[l].stl_start -= n;
		    if (stl_items[l].stl_start < t)
			stl_items[l].stl_start = t;
		}
	    }
	    else if (abs(stl_items[stl_groupitem[groupdepth]].stl_minwid) > l)
	    {
		// fill
		n = stl_items[stl_groupitem[groupdepth]].stl_minwid;
		if (n < 0)
		{
		    // fill by appending characters
		    n = 0 - n;
		    while (l++ < n && p + 1 < out + outlen)
			MB_CHAR2BYTES(fillchar, p);
		}
		else
		{
		    // fill by inserting characters
		    l = (n - l) * MB_CHAR2LEN(fillchar);
		    mch_memmove(t + l, t, (size_t)(p - t));
		    if (p + l >= out + outlen)
			l = (long)((out + outlen) - p - 1);
		    p += l;
		    for (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
			stl_items[n].stl_start += l;
		    for ( ; l > 0; l--)
			MB_CHAR2BYTES(fillchar, t);
		}
	    }
	    continue;
	}
	minwid = 0;
	maxwid = 9999;
	zeropad = FALSE;
	l = 1;
	if (*s == '0')
	{
	    s++;
	    zeropad = TRUE;
	}
	if (*s == '-')
	{
	    s++;
	    l = -1;
	}
	if (VIM_ISDIGIT(*s))
	{
	    minwid = (int)getdigits(&s);
	    if (minwid < 0)	// overflow
		minwid = 0;
	}
	if (*s == STL_USER_HL)
	{
	    stl_items[curitem].stl_type = Highlight;
	    stl_items[curitem].stl_start = p;
	    stl_items[curitem].stl_minwid = minwid > 9 ? 1 : minwid;
	    s++;
	    curitem++;
	    continue;
	}
	if (*s == STL_TABPAGENR || *s == STL_TABCLOSENR)
	{
	    if (*s == STL_TABCLOSENR)
	    {
		if (minwid == 0)
		{
		    // %X ends the close label, go back to the previously
		    // define tab label nr.
		    for (n = curitem - 1; n >= 0; --n)
			if (stl_items[n].stl_type == TabPage
					       && stl_items[n].stl_minwid >= 0)
			{
			    minwid = stl_items[n].stl_minwid;
			    break;
			}
		}
		else
		    // close nrs are stored as negative values
		    minwid = - minwid;
	    }
	    stl_items[curitem].stl_type = TabPage;
	    stl_items[curitem].stl_start = p;
	    stl_items[curitem].stl_minwid = minwid;
	    s++;
	    curitem++;
	    continue;
	}
	if (*s == '.')
	{
	    s++;
	    if (VIM_ISDIGIT(*s))
	    {
		maxwid = (int)getdigits(&s);
		if (maxwid <= 0)	// overflow
		    maxwid = 50;
	    }
	}
	minwid = (minwid > 50 ? 50 : minwid) * l;
	if (*s == '(')
	{
	    stl_groupitem[groupdepth++] = curitem;
	    stl_items[curitem].stl_type = Group;
	    stl_items[curitem].stl_start = p;
	    stl_items[curitem].stl_minwid = minwid;
	    stl_items[curitem].stl_maxwid = maxwid;
	    s++;
	    curitem++;
	    continue;
	}
#ifdef FEAT_EVAL
	// Denotes end of expanded %{} block
	if (*s == '}' && evaldepth > 0)
	{
	    s++;
	    evaldepth--;
	    continue;
	}
#endif
	if (vim_strchr(STL_ALL, *s) == NULL)
	{
	    s++;
	    continue;
	}
	opt = *s++;

	// OK - now for the real work
	base = 'D';
	itemisflag = FALSE;
	fillable = TRUE;
	num = -1;
	str = NULL;
	switch (opt)
	{
	case STL_FILEPATH:
	case STL_FULLPATH:
	case STL_FILENAME:
	    fillable = FALSE;	// don't change ' ' to fillchar
	    if (buf_spname(wp->w_buffer) != NULL)
		vim_strncpy(NameBuff, buf_spname(wp->w_buffer), MAXPATHL - 1);
	    else
	    {
		t = (opt == STL_FULLPATH) ? wp->w_buffer->b_ffname
					  : wp->w_buffer->b_fname;
		home_replace(wp->w_buffer, t, NameBuff, MAXPATHL, TRUE);
	    }
	    trans_characters(NameBuff, MAXPATHL);
	    if (opt != STL_FILENAME)
		str = NameBuff;
	    else
		str = gettail(NameBuff);
	    break;

	case STL_VIM_EXPR: // '{'
	{
#ifdef FEAT_EVAL
	    char_u *block_start = s - 1;
#endif
	    int reevaluate = (*s == '%');

	    if (reevaluate)
		s++;
	    itemisflag = TRUE;
	    t = p;
	    while ((*s != '}' || (reevaluate && s[-1] != '%'))
					  && *s != NUL && p + 1 < out + outlen)
		*p++ = *s++;
	    if (*s != '}')	// missing '}' or out of space
		break;
	    s++;
	    if (reevaluate)
		p[-1] = 0; // remove the % at the end of %{% expr %}
	    else
		*p = 0;
	    p = t;
#ifdef FEAT_EVAL
	    vim_snprintf((char *)buf_tmp, sizeof(buf_tmp),
							 "%d", curbuf->b_fnum);
	    set_internal_string_var((char_u *)"g:actual_curbuf", buf_tmp);
	    vim_snprintf((char *)win_tmp, sizeof(win_tmp), "%d", curwin->w_id);
	    set_internal_string_var((char_u *)"g:actual_curwin", win_tmp);

	    save_curbuf = curbuf;
	    save_curwin = curwin;
	    save_VIsual_active = VIsual_active;
	    curwin = wp;
	    curbuf = wp->w_buffer;
	    // Visual mode is only valid in the current window.
	    if (curwin != save_curwin)
		VIsual_active = FALSE;

	    str = eval_to_string_safe(p, use_sandbox, FALSE);

	    curwin = save_curwin;
	    curbuf = save_curbuf;
	    VIsual_active = save_VIsual_active;
	    do_unlet((char_u *)"g:actual_curbuf", TRUE);
	    do_unlet((char_u *)"g:actual_curwin", TRUE);

	    if (str != NULL && *str != 0)
	    {
		if (*skipdigits(str) == NUL)
		{
		    num = atoi((char *)str);
		    VIM_CLEAR(str);
		    itemisflag = FALSE;
		}
	    }

	    // If the output of the expression needs to be evaluated
	    // replace the %{} block with the result of evaluation
	    if (reevaluate && str != NULL && *str != 0
		    && strchr((const char *)str, '%') != NULL
		    && evaldepth < MAX_STL_EVAL_DEPTH)
	    {
		size_t parsed_usefmt = (size_t)(block_start - usefmt);
		size_t str_length = strlen((const char *)str);
		size_t fmt_length = strlen((const char *)s);
		size_t new_fmt_len = parsed_usefmt
						 + str_length + fmt_length + 3;
		char_u *new_fmt = (char_u *)alloc(new_fmt_len * sizeof(char_u));
		char_u *new_fmt_p = new_fmt;

		new_fmt_p = (char_u *)memcpy(new_fmt_p, usefmt, parsed_usefmt)
							       + parsed_usefmt;
		new_fmt_p = (char_u *)memcpy(new_fmt_p , str, str_length)
								  + str_length;
		new_fmt_p = (char_u *)memcpy(new_fmt_p, "%}", 2) + 2;
		new_fmt_p = (char_u *)memcpy(new_fmt_p , s, fmt_length)
								  + fmt_length;
		*new_fmt_p = 0;
		new_fmt_p = NULL;

		if (usefmt != fmt)
		    vim_free(usefmt);
		VIM_CLEAR(str);
		usefmt = new_fmt;
		s = usefmt + parsed_usefmt;
		evaldepth++;
		continue;
	    }
#endif
	    break;
	}
	case STL_LINE:
	    num = (wp->w_buffer->b_ml.ml_flags & ML_EMPTY)
		  ? 0L : (long)(wp->w_cursor.lnum);
	    break;

	case STL_NUMLINES:
	    num = wp->w_buffer->b_ml.ml_line_count;
	    break;

	case STL_COLUMN:
	    num = !(State & INSERT) && empty_line
		  ? 0 : (int)wp->w_cursor.col + 1;
	    break;

	case STL_VIRTCOL:
	case STL_VIRTCOL_ALT:
	    virtcol = wp->w_virtcol + 1;
	    // Don't display %V if it's the same as %c.
	    if (opt == STL_VIRTCOL_ALT
		    && (virtcol == (colnr_T)(!(State & INSERT) && empty_line
			    ? 0 : (int)wp->w_cursor.col + 1)))
		break;
	    num = (long)virtcol;
	    break;

	case STL_PERCENTAGE:
	    num = (int)(((long)wp->w_cursor.lnum * 100L) /
			(long)wp->w_buffer->b_ml.ml_line_count);
	    break;

	case STL_ALTPERCENT:
	    str = buf_tmp;
	    get_rel_pos(wp, str, TMPLEN);
	    break;

	case STL_ARGLISTSTAT:
	    fillable = FALSE;
	    buf_tmp[0] = 0;
	    if (append_arg_number(wp, buf_tmp, (int)sizeof(buf_tmp), FALSE))
		str = buf_tmp;
	    break;

	case STL_KEYMAP:
	    fillable = FALSE;
	    if (get_keymap_str(wp, (char_u *)"<%s>", buf_tmp, TMPLEN))
		str = buf_tmp;
	    break;
	case STL_PAGENUM:
#if defined(FEAT_PRINTER) || defined(FEAT_GUI_TABLINE)
	    num = printer_page_num;
#else
	    num = 0;
#endif
	    break;

	case STL_BUFNO:
	    num = wp->w_buffer->b_fnum;
	    break;

	case STL_OFFSET_X:
	    base = 'X';
	    // FALLTHROUGH
	case STL_OFFSET:
#ifdef FEAT_BYTEOFF
	    l = ml_find_line_or_offset(wp->w_buffer, wp->w_cursor.lnum, NULL);
	    num = (wp->w_buffer->b_ml.ml_flags & ML_EMPTY) || l < 0 ?
		  0L : l + 1 + (!(State & INSERT) && empty_line ?
				0 : (int)wp->w_cursor.col);
#endif
	    break;

	case STL_BYTEVAL_X:
	    base = 'X';
	    // FALLTHROUGH
	case STL_BYTEVAL:
	    num = byteval;
	    if (num == NL)
		num = 0;
	    else if (num == CAR && get_fileformat(wp->w_buffer) == EOL_MAC)
		num = NL;
	    break;

	case STL_ROFLAG:
	case STL_ROFLAG_ALT:
	    itemisflag = TRUE;
	    if (wp->w_buffer->b_p_ro)
		str = (char_u *)((opt == STL_ROFLAG_ALT) ? ",RO" : _("[RO]"));
	    break;

	case STL_HELPFLAG:
	case STL_HELPFLAG_ALT:
	    itemisflag = TRUE;
	    if (wp->w_buffer->b_help)
		str = (char_u *)((opt == STL_HELPFLAG_ALT) ? ",HLP"
							       : _("[Help]"));
	    break;

	case STL_FILETYPE:
	    if (*wp->w_buffer->b_p_ft != NUL
		    && STRLEN(wp->w_buffer->b_p_ft) < TMPLEN - 3)
	    {
		vim_snprintf((char *)buf_tmp, sizeof(buf_tmp), "[%s]",
							wp->w_buffer->b_p_ft);
		str = buf_tmp;
	    }
	    break;

	case STL_FILETYPE_ALT:
	    itemisflag = TRUE;
	    if (*wp->w_buffer->b_p_ft != NUL
		    && STRLEN(wp->w_buffer->b_p_ft) < TMPLEN - 2)
	    {
		vim_snprintf((char *)buf_tmp, sizeof(buf_tmp), ",%s",
							wp->w_buffer->b_p_ft);
		for (t = buf_tmp; *t != 0; t++)
		    *t = TOUPPER_LOC(*t);
		str = buf_tmp;
	    }
	    break;

#if defined(FEAT_QUICKFIX)
	case STL_PREVIEWFLAG:
	case STL_PREVIEWFLAG_ALT:
	    itemisflag = TRUE;
	    if (wp->w_p_pvw)
		str = (char_u *)((opt == STL_PREVIEWFLAG_ALT) ? ",PRV"
							    : _("[Preview]"));
	    break;

	case STL_QUICKFIX:
	    if (bt_quickfix(wp->w_buffer))
		str = (char_u *)(wp->w_llist_ref
			    ? _(msg_loclist)
			    : _(msg_qflist));
	    break;
#endif

	case STL_MODIFIED:
	case STL_MODIFIED_ALT:
	    itemisflag = TRUE;
	    switch ((opt == STL_MODIFIED_ALT)
		    + bufIsChanged(wp->w_buffer) * 2
		    + (!wp->w_buffer->b_p_ma) * 4)
	    {
		case 2: str = (char_u *)"[+]"; break;
		case 3: str = (char_u *)",+"; break;
		case 4: str = (char_u *)"[-]"; break;
		case 5: str = (char_u *)",-"; break;
		case 6: str = (char_u *)"[+-]"; break;
		case 7: str = (char_u *)",+-"; break;
	    }
	    break;

	case STL_HIGHLIGHT:
	    t = s;
	    while (*s != '#' && *s != NUL)
		++s;
	    if (*s == '#')
	    {
		stl_items[curitem].stl_type = Highlight;
		stl_items[curitem].stl_start = p;
		stl_items[curitem].stl_minwid = -syn_namen2id(t, (int)(s - t));
		curitem++;
	    }
	    if (*s != NUL)
		++s;
	    continue;
	}

	stl_items[curitem].stl_start = p;
	stl_items[curitem].stl_type = Normal;
	if (str != NULL && *str)
	{
	    t = str;
	    if (itemisflag)
	    {
		if ((t[0] && t[1])
			&& ((!prevchar_isitem && *t == ',')
			      || (prevchar_isflag && *t == ' ')))
		    t++;
		prevchar_isflag = TRUE;
	    }
	    l = vim_strsize(t);
	    if (l > 0)
		prevchar_isitem = TRUE;
	    if (l > maxwid)
	    {
		while (l >= maxwid)
		    if (has_mbyte)
		    {
			l -= ptr2cells(t);
			t += (*mb_ptr2len)(t);
		    }
		    else
			l -= byte2cells(*t++);
		if (p + 1 >= out + outlen)
		    break;
		*p++ = '<';
	    }
	    if (minwid > 0)
	    {
		for (; l < minwid && p + 1 < out + outlen; l++)
		{
		    // Don't put a "-" in front of a digit.
		    if (l + 1 == minwid && fillchar == '-' && VIM_ISDIGIT(*t))
			*p++ = ' ';
		    else
			MB_CHAR2BYTES(fillchar, p);
		}
		minwid = 0;
	    }
	    else
		minwid *= -1;
	    for (; *t && p + 1 < out + outlen; t++)
	    {
		// Change a space by fillchar, unless fillchar is '-' and a
		// digit follows.
		if (fillable && *t == ' '
				&& (!VIM_ISDIGIT(*(t + 1)) || fillchar != '-'))
		    MB_CHAR2BYTES(fillchar, p);
		else
		    *p++ = *t;
	    }
	    for (; l < minwid && p + 1 < out + outlen; l++)
		MB_CHAR2BYTES(fillchar, p);
	}
	else if (num >= 0)
	{
	    int nbase = (base == 'D' ? 10 : (base == 'O' ? 8 : 16));
	    char_u nstr[20];

	    if (p + 20 >= out + outlen)
		break;		// not sufficient space
	    prevchar_isitem = TRUE;
	    t = nstr;
	    if (opt == STL_VIRTCOL_ALT)
	    {
		*t++ = '-';
		minwid--;
	    }
	    *t++ = '%';
	    if (zeropad)
		*t++ = '0';
	    *t++ = '*';
	    *t++ = nbase == 16 ? base : (char_u)(nbase == 8 ? 'o' : 'd');
	    *t = 0;

	    for (n = num, l = 1; n >= nbase; n /= nbase)
		l++;
	    if (opt == STL_VIRTCOL_ALT)
		l++;
	    if (l > maxwid)
	    {
		l += 2;
		n = l - maxwid;
		while (l-- > maxwid)
		    num /= nbase;
		*t++ = '>';
		*t++ = '%';
		*t = t[-3];
		*++t = 0;
		vim_snprintf((char *)p, outlen - (p - out), (char *)nstr,
								   0, num, n);
	    }
	    else
		vim_snprintf((char *)p, outlen - (p - out), (char *)nstr,
								 minwid, num);
	    p += STRLEN(p);
	}
	else
	    stl_items[curitem].stl_type = Empty;

	if (opt == STL_VIM_EXPR)
	    vim_free(str);

	if (num >= 0 || (!itemisflag && str && *str))
	    prevchar_isflag = FALSE;	    // Item not NULL, but not a flag
	curitem++;
    }
    *p = NUL;
    itemcnt = curitem;

#ifdef FEAT_EVAL
    if (usefmt != fmt)
	vim_free(usefmt);
#endif

    width = vim_strsize(out);
    if (maxwidth > 0 && width > maxwidth)
    {
	// Result is too long, must truncate somewhere.
	l = 0;
	if (itemcnt == 0)
	    s = out;
	else
	{
	    for ( ; l < itemcnt; l++)
		if (stl_items[l].stl_type == Trunc)
		{
		    // Truncate at %< item.
		    s = stl_items[l].stl_start;
		    break;
		}
	    if (l == itemcnt)
	    {
		// No %< item, truncate first item.
		s = stl_items[0].stl_start;
		l = 0;
	    }
	}

	if (width - vim_strsize(s) >= maxwidth)
	{
	    // Truncation mark is beyond max length
	    if (has_mbyte)
	    {
		s = out;
		width = 0;
		for (;;)
		{
		    width += ptr2cells(s);
		    if (width >= maxwidth)
			break;
		    s += (*mb_ptr2len)(s);
		}
		// Fill up for half a double-wide character.
		while (++width < maxwidth)
		    MB_CHAR2BYTES(fillchar, s);
	    }
	    else
		s = out + maxwidth - 1;
	    for (l = 0; l < itemcnt; l++)
		if (stl_items[l].stl_start > s)
		    break;
	    itemcnt = l;
	    *s++ = '>';
	    *s = 0;
	}
	else
	{
	    if (has_mbyte)
	    {
		n = 0;
		while (width >= maxwidth)
		{
		    width -= ptr2cells(s + n);
		    n += (*mb_ptr2len)(s + n);
		}
	    }
	    else
		n = width - maxwidth + 1;
	    p = s + n;
	    STRMOVE(s + 1, p);
	    *s = '<';

	    // Fill up for half a double-wide character.
	    while (++width < maxwidth)
	    {
		s = s + STRLEN(s);
		MB_CHAR2BYTES(fillchar, s);
		*s = NUL;
	    }

	    --n;	// count the '<'
	    for (; l < itemcnt; l++)
	    {
		if (stl_items[l].stl_start - n >= s)
		    stl_items[l].stl_start -= n;
		else
		    stl_items[l].stl_start = s;
	    }
	}
	width = maxwidth;
    }
    else if (width < maxwidth && STRLEN(out) + maxwidth - width + 1 < outlen)
    {
	// Apply STL_MIDDLE if any
	for (l = 0; l < itemcnt; l++)
	    if (stl_items[l].stl_type == Middle)
		break;
	if (l < itemcnt)
	{
	    int middlelength = (maxwidth - width) * MB_CHAR2LEN(fillchar);
	    p = stl_items[l].stl_start + middlelength;
	    STRMOVE(p, stl_items[l].stl_start);
	    for (s = stl_items[l].stl_start; s < p;)
		MB_CHAR2BYTES(fillchar, s);
	    for (l++; l < itemcnt; l++)
		stl_items[l].stl_start += middlelength;
	    width = maxwidth;
	}
    }

    // Store the info about highlighting.
    if (hltab != NULL)
    {
	*hltab = stl_hltab;
	sp = stl_hltab;
	for (l = 0; l < itemcnt; l++)
	{
	    if (stl_items[l].stl_type == Highlight)
	    {
		sp->start = stl_items[l].stl_start;
		sp->userhl = stl_items[l].stl_minwid;
		sp++;
	    }
	}
	sp->start = NULL;
	sp->userhl = 0;
    }

    // Store the info about tab pages labels.
    if (tabtab != NULL)
    {
	*tabtab = stl_tabtab;
	sp = stl_tabtab;
	for (l = 0; l < itemcnt; l++)
	{
	    if (stl_items[l].stl_type == TabPage)
	    {
		sp->start = stl_items[l].stl_start;
		sp->userhl = stl_items[l].stl_minwid;
		sp++;
	    }
	}
	sp->start = NULL;
	sp->userhl = 0;
    }

    // When inside update_screen we do not want redrawing a statusline, ruler,
    // title, etc. to trigger another redraw, it may cause an endless loop.
    if (updating_screen)
    {
	must_redraw = save_must_redraw;
	curwin->w_redr_type = save_redr_type;
    }

    // A user function may reset KeyTyped, restore it.
    KeyTyped = save_KeyTyped;

    return width;
}