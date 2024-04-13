shift_block(oparg_T *oap, int amount)
{
    int			left = (oap->op_type == OP_LSHIFT);
    int			oldstate = State;
    int			total;
    char_u		*newp, *oldp;
    int			oldcol = curwin->w_cursor.col;
    int			sw_val = (int)get_sw_value_indent(curbuf);
    int			ts_val = (int)curbuf->b_p_ts;
    struct block_def	bd;
    int			incr;
    colnr_T		ws_vcol;
    int			i = 0, j = 0;
    int			len;
#ifdef FEAT_RIGHTLEFT
    int			old_p_ri = p_ri;

    p_ri = 0;			// don't want revins in indent
#endif

    State = INSERT;		// don't want REPLACE for State
    block_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
    if (bd.is_short)
	return;

    // total is number of screen columns to be inserted/removed
    total = (int)((unsigned)amount * (unsigned)sw_val);
    if ((total / sw_val) != amount)
	return; // multiplication overflow

    oldp = ml_get_curline();

    if (!left)
    {
	/*
	 *  1. Get start vcol
	 *  2. Total ws vcols
	 *  3. Divvy into TABs & spp
	 *  4. Construct new string
	 */
	total += bd.pre_whitesp; // all virtual WS up to & incl a split TAB
	ws_vcol = bd.start_vcol - bd.pre_whitesp;
	if (bd.startspaces)
	{
	    if (has_mbyte)
	    {
		if ((*mb_ptr2len)(bd.textstart) == 1)
		    ++bd.textstart;
		else
		{
		    ws_vcol = 0;
		    bd.startspaces = 0;
		}
	    }
	    else
		++bd.textstart;
	}
	for ( ; VIM_ISWHITE(*bd.textstart); )
	{
	    // TODO: is passing bd.textstart for start of the line OK?
	    incr = lbr_chartabsize_adv(bd.textstart, &bd.textstart,
						    (colnr_T)(bd.start_vcol));
	    total += incr;
	    bd.start_vcol += incr;
	}
	// OK, now total=all the VWS reqd, and textstart points at the 1st
	// non-ws char in the block.
#ifdef FEAT_VARTABS
	if (!curbuf->b_p_et)
	    tabstop_fromto(ws_vcol, ws_vcol + total,
					ts_val, curbuf->b_p_vts_array, &i, &j);
	else
	    j = total;
#else
	if (!curbuf->b_p_et)
	    i = ((ws_vcol % ts_val) + total) / ts_val; // number of tabs
	if (i)
	    j = ((ws_vcol % ts_val) + total) % ts_val; // number of spp
	else
	    j = total;
#endif
	// if we're splitting a TAB, allow for it
	bd.textcol -= bd.pre_whitesp_c - (bd.startspaces != 0);
	len = (int)STRLEN(bd.textstart) + 1;
	newp = alloc(bd.textcol + i + j + len);
	if (newp == NULL)
	    return;
	vim_memset(newp, NUL, (size_t)(bd.textcol + i + j + len));
	mch_memmove(newp, oldp, (size_t)bd.textcol);
	vim_memset(newp + bd.textcol, TAB, (size_t)i);
	vim_memset(newp + bd.textcol + i, ' ', (size_t)j);
	// the end
	mch_memmove(newp + bd.textcol + i + j, bd.textstart, (size_t)len);
    }
    else // left
    {
	colnr_T	    destination_col;	// column to which text in block will
					// be shifted
	char_u	    *verbatim_copy_end;	// end of the part of the line which is
					// copied verbatim
	colnr_T	    verbatim_copy_width;// the (displayed) width of this part
					// of line
	unsigned    fill;		// nr of spaces that replace a TAB
	unsigned    new_line_len;	// the length of the line after the
					// block shift
	size_t	    block_space_width;
	size_t	    shift_amount;
	char_u	    *non_white = bd.textstart;
	colnr_T	    non_white_col;

	/*
	 * Firstly, let's find the first non-whitespace character that is
	 * displayed after the block's start column and the character's column
	 * number. Also, let's calculate the width of all the whitespace
	 * characters that are displayed in the block and precede the searched
	 * non-whitespace character.
	 */

	// If "bd.startspaces" is set, "bd.textstart" points to the character,
	// the part of which is displayed at the block's beginning. Let's start
	// searching from the next character.
	if (bd.startspaces)
	    MB_PTR_ADV(non_white);

	// The character's column is in "bd.start_vcol".
	non_white_col = bd.start_vcol;

	while (VIM_ISWHITE(*non_white))
	{
	    incr = lbr_chartabsize_adv(bd.textstart, &non_white, non_white_col);
	    non_white_col += incr;
	}

	block_space_width = non_white_col - oap->start_vcol;
	// We will shift by "total" or "block_space_width", whichever is less.
	shift_amount = (block_space_width < (size_t)total
					 ? block_space_width : (size_t)total);

	// The column to which we will shift the text.
	destination_col = (colnr_T)(non_white_col - shift_amount);

	// Now let's find out how much of the beginning of the line we can
	// reuse without modification.
	verbatim_copy_end = bd.textstart;
	verbatim_copy_width = bd.start_vcol;

	// If "bd.startspaces" is set, "bd.textstart" points to the character
	// preceding the block. We have to subtract its width to obtain its
	// column number.
	if (bd.startspaces)
	    verbatim_copy_width -= bd.start_char_vcols;
	while (verbatim_copy_width < destination_col)
	{
	    char_u *line = verbatim_copy_end;

	    // TODO: is passing verbatim_copy_end for start of the line OK?
	    incr = lbr_chartabsize(line, verbatim_copy_end,
							 verbatim_copy_width);
	    if (verbatim_copy_width + incr > destination_col)
		break;
	    verbatim_copy_width += incr;
	    MB_PTR_ADV(verbatim_copy_end);
	}

	// If "destination_col" is different from the width of the initial
	// part of the line that will be copied, it means we encountered a tab
	// character, which we will have to partly replace with spaces.
	fill = destination_col - verbatim_copy_width;

	// The replacement line will consist of:
	// - the beginning of the original line up to "verbatim_copy_end",
	// - "fill" number of spaces,
	// - the rest of the line, pointed to by non_white.
	new_line_len = (unsigned)(verbatim_copy_end - oldp)
		       + fill
		       + (unsigned)STRLEN(non_white) + 1;

	newp = alloc(new_line_len);
	if (newp == NULL)
	    return;
	mch_memmove(newp, oldp, (size_t)(verbatim_copy_end - oldp));
	vim_memset(newp + (verbatim_copy_end - oldp), ' ', (size_t)fill);
	STRMOVE(newp + (verbatim_copy_end - oldp) + fill, non_white);
    }
    // replace the line
    ml_replace(curwin->w_cursor.lnum, newp, FALSE);
    changed_bytes(curwin->w_cursor.lnum, (colnr_T)bd.textcol);
    State = oldstate;
    curwin->w_cursor.col = oldcol;
#ifdef FEAT_RIGHTLEFT
    p_ri = old_p_ri;
#endif
}