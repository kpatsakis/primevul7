vgetorpeek(int advance)
{
    int		c, c1;
    int		keylen;
    char_u	*s;
    mapblock_T	*mp;
#ifdef FEAT_LOCALMAP
    mapblock_T	*mp2;
#endif
    mapblock_T	*mp_match;
    int		mp_match_len = 0;
    int		timedout = FALSE;	    /* waited for more than 1 second
						for mapping to complete */
    int		mapdepth = 0;	    /* check for recursive mapping */
    int		mode_deleted = FALSE;   /* set when mode has been deleted */
    int		local_State;
    int		mlen;
    int		max_mlen;
    int		i;
#ifdef FEAT_CMDL_INFO
    int		new_wcol, new_wrow;
#endif
#ifdef FEAT_GUI
# ifdef FEAT_MENU
    int		idx;
# endif
    int		shape_changed = FALSE;  /* adjusted cursor shape */
#endif
    int		n;
#ifdef FEAT_LANGMAP
    int		nolmaplen;
#endif
    int		old_wcol, old_wrow;
    int		wait_tb_len;

    /*
     * This function doesn't work very well when called recursively.  This may
     * happen though, because of:
     * 1. The call to add_to_showcmd().	char_avail() is then used to check if
     * there is a character available, which calls this function.  In that
     * case we must return NUL, to indicate no character is available.
     * 2. A GUI callback function writes to the screen, causing a
     * wait_return().
     * Using ":normal" can also do this, but it saves the typeahead buffer,
     * thus it should be OK.  But don't get a key from the user then.
     */
    if (vgetc_busy > 0 && ex_normal_busy == 0)
	return NUL;

    local_State = get_real_state();

    ++vgetc_busy;

    if (advance)
	KeyStuffed = FALSE;

    init_typebuf();
    start_stuff();
    if (advance && typebuf.tb_maplen == 0)
	reg_executing = 0;
    do
    {
/*
 * get a character: 1. from the stuffbuffer
 */
	if (typeahead_char != 0)
	{
	    c = typeahead_char;
	    if (advance)
		typeahead_char = 0;
	}
	else
	    c = read_readbuffers(advance);
	if (c != NUL && !got_int)
	{
	    if (advance)
	    {
		/* KeyTyped = FALSE;  When the command that stuffed something
		 * was typed, behave like the stuffed command was typed.
		 * needed for CTRL-W CTRL-] to open a fold, for example. */
		KeyStuffed = TRUE;
	    }
	    if (typebuf.tb_no_abbr_cnt == 0)
		typebuf.tb_no_abbr_cnt = 1;	/* no abbreviations now */
	}
	else
	{
	    /*
	     * Loop until we either find a matching mapped key, or we
	     * are sure that it is not a mapped key.
	     * If a mapped key sequence is found we go back to the start to
	     * try re-mapping.
	     */
	    for (;;)
	    {
		long	    wait_time;

		/*
		 * ui_breakcheck() is slow, don't use it too often when
		 * inside a mapping.  But call it each time for typed
		 * characters.
		 */
		if (typebuf.tb_maplen)
		    line_breakcheck();
		else
		    ui_breakcheck();		/* check for CTRL-C */
		keylen = 0;
		if (got_int)
		{
		    /* flush all input */
		    c = inchar(typebuf.tb_buf, typebuf.tb_buflen - 1, 0L);
		    /*
		     * If inchar() returns TRUE (script file was active) or we
		     * are inside a mapping, get out of Insert mode.
		     * Otherwise we behave like having gotten a CTRL-C.
		     * As a result typing CTRL-C in insert mode will
		     * really insert a CTRL-C.
		     */
		    if ((c || typebuf.tb_maplen)
					      && (State & (INSERT + CMDLINE)))
			c = ESC;
		    else
			c = Ctrl_C;
		    flush_buffers(FLUSH_INPUT);	// flush all typeahead

		    if (advance)
		    {
			/* Also record this character, it might be needed to
			 * get out of Insert mode. */
			*typebuf.tb_buf = c;
			gotchars(typebuf.tb_buf, 1);
		    }
		    cmd_silent = FALSE;

		    break;
		}
		else if (typebuf.tb_len > 0)
		{
		    /*
		     * Check for a mappable key sequence.
		     * Walk through one maphash[] list until we find an
		     * entry that matches.
		     *
		     * Don't look for mappings if:
		     * - no_mapping set: mapping disabled (e.g. for CTRL-V)
		     * - maphash_valid not set: no mappings present.
		     * - typebuf.tb_buf[typebuf.tb_off] should not be remapped
		     * - in insert or cmdline mode and 'paste' option set
		     * - waiting for "hit return to continue" and CR or SPACE
		     *	 typed
		     * - waiting for a char with --more--
		     * - in Ctrl-X mode, and we get a valid char for that mode
		     */
		    mp = NULL;
		    max_mlen = 0;
		    c1 = typebuf.tb_buf[typebuf.tb_off];
		    if (no_mapping == 0 && maphash_valid
			    && (no_zero_mapping == 0 || c1 != '0')
			    && (typebuf.tb_maplen == 0
				|| (p_remap
				    && (typebuf.tb_noremap[typebuf.tb_off]
						    & (RM_NONE|RM_ABBR)) == 0))
			    && !(p_paste && (State & (INSERT + CMDLINE)))
			    && !(State == HITRETURN && (c1 == CAR || c1 == ' '))
			    && State != ASKMORE
			    && State != CONFIRM
#ifdef FEAT_INS_EXPAND
			    && !((ctrl_x_mode_not_default()
						      && vim_is_ctrl_x_key(c1))
				    || ((compl_cont_status & CONT_LOCAL)
					&& (c1 == Ctrl_N || c1 == Ctrl_P)))
#endif
			    )
		    {
#ifdef FEAT_LANGMAP
			if (c1 == K_SPECIAL)
			    nolmaplen = 2;
			else
			{
			    LANGMAP_ADJUST(c1,
					   (State & (CMDLINE | INSERT)) == 0
					   && get_real_state() != SELECTMODE);
			    nolmaplen = 0;
			}
#endif
#ifdef FEAT_LOCALMAP
			/* First try buffer-local mappings. */
			mp = curbuf->b_maphash[MAP_HASH(local_State, c1)];
			mp2 = maphash[MAP_HASH(local_State, c1)];
			if (mp == NULL)
			{
			    /* There are no buffer-local mappings. */
			    mp = mp2;
			    mp2 = NULL;
			}
#else
			mp = maphash[MAP_HASH(local_State, c1)];
#endif
			/*
			 * Loop until a partly matching mapping is found or
			 * all (local) mappings have been checked.
			 * The longest full match is remembered in "mp_match".
			 * A full match is only accepted if there is no partly
			 * match, so "aa" and "aaa" can both be mapped.
			 */
			mp_match = NULL;
			mp_match_len = 0;
			for ( ; mp != NULL;
#ifdef FEAT_LOCALMAP
				mp->m_next == NULL ? (mp = mp2, mp2 = NULL) :
#endif
				(mp = mp->m_next))
			{
			    /*
			     * Only consider an entry if the first character
			     * matches and it is for the current state.
			     * Skip ":lmap" mappings if keys were mapped.
			     */
			    if (mp->m_keys[0] == c1
				    && (mp->m_mode & local_State)
				    && ((mp->m_mode & LANGMAP) == 0
					|| typebuf.tb_maplen == 0))
			    {
#ifdef FEAT_LANGMAP
				int	nomap = nolmaplen;
				int	c2;
#endif
				/* find the match length of this mapping */
				for (mlen = 1; mlen < typebuf.tb_len; ++mlen)
				{
#ifdef FEAT_LANGMAP
				    c2 = typebuf.tb_buf[typebuf.tb_off + mlen];
				    if (nomap > 0)
					--nomap;
				    else if (c2 == K_SPECIAL)
					nomap = 2;
				    else
					LANGMAP_ADJUST(c2, TRUE);
				    if (mp->m_keys[mlen] != c2)
#else
				    if (mp->m_keys[mlen] !=
					typebuf.tb_buf[typebuf.tb_off + mlen])
#endif
					break;
				}

				/* Don't allow mapping the first byte(s) of a
				 * multi-byte char.  Happens when mapping
				 * <M-a> and then changing 'encoding'. Beware
				 * that 0x80 is escaped. */
				{
				    char_u *p1 = mp->m_keys;
				    char_u *p2 = mb_unescape(&p1);

				    if (has_mbyte && p2 != NULL
					  && MB_BYTE2LEN(c1) > MB_PTR2LEN(p2))
					mlen = 0;
				}
				/*
				 * Check an entry whether it matches.
				 * - Full match: mlen == keylen
				 * - Partly match: mlen == typebuf.tb_len
				 */
				keylen = mp->m_keylen;
				if (mlen == keylen
				     || (mlen == typebuf.tb_len
						  && typebuf.tb_len < keylen))
				{
				    /*
				     * If only script-local mappings are
				     * allowed, check if the mapping starts
				     * with K_SNR.
				     */
				    s = typebuf.tb_noremap + typebuf.tb_off;
				    if (*s == RM_SCRIPT
					    && (mp->m_keys[0] != K_SPECIAL
						|| mp->m_keys[1] != KS_EXTRA
						|| mp->m_keys[2]
							      != (int)KE_SNR))
					continue;
				    /*
				     * If one of the typed keys cannot be
				     * remapped, skip the entry.
				     */
				    for (n = mlen; --n >= 0; )
					if (*s++ & (RM_NONE|RM_ABBR))
					    break;
				    if (n >= 0)
					continue;

				    if (keylen > typebuf.tb_len)
				    {
					if (!timedout && !(mp_match != NULL
						       && mp_match->m_nowait))
					{
					    /* break at a partly match */
					    keylen = KEYLEN_PART_MAP;
					    break;
					}
				    }
				    else if (keylen > mp_match_len)
				    {
					/* found a longer match */
					mp_match = mp;
					mp_match_len = keylen;
				    }
				}
				else
				    /* No match; may have to check for
				     * termcode at next character. */
				    if (max_mlen < mlen)
					max_mlen = mlen;
			    }
			}

			/* If no partly match found, use the longest full
			 * match. */
			if (keylen != KEYLEN_PART_MAP)
			{
			    mp = mp_match;
			    keylen = mp_match_len;
			}
		    }

		    /* Check for match with 'pastetoggle' */
		    if (*p_pt != NUL && mp == NULL && (State & (INSERT|NORMAL)))
		    {
			for (mlen = 0; mlen < typebuf.tb_len && p_pt[mlen];
								       ++mlen)
			    if (p_pt[mlen] != typebuf.tb_buf[typebuf.tb_off
								      + mlen])
				    break;
			if (p_pt[mlen] == NUL)	/* match */
			{
			    /* write chars to script file(s) */
			    if (mlen > typebuf.tb_maplen)
				gotchars(typebuf.tb_buf + typebuf.tb_off
							  + typebuf.tb_maplen,
						    mlen - typebuf.tb_maplen);

			    del_typebuf(mlen, 0); /* remove the chars */
			    set_option_value((char_u *)"paste",
						     (long)!p_paste, NULL, 0);
			    if (!(State & INSERT))
			    {
				msg_col = 0;
				msg_row = Rows - 1;
				msg_clr_eos();		/* clear ruler */
			    }
			    status_redraw_all();
			    redraw_statuslines();
			    showmode();
			    setcursor();
			    continue;
			}
			/* Need more chars for partly match. */
			if (mlen == typebuf.tb_len)
			    keylen = KEYLEN_PART_KEY;
			else if (max_mlen < mlen)
			    /* no match, may have to check for termcode at
			     * next character */
			    max_mlen = mlen + 1;
		    }

		    if ((mp == NULL || max_mlen >= mp_match_len)
						 && keylen != KEYLEN_PART_MAP)
		    {
			int	save_keylen = keylen;

			/*
			 * When no matching mapping found or found a
			 * non-matching mapping that matches at least what the
			 * matching mapping matched:
			 * Check if we have a terminal code, when:
			 *  mapping is allowed,
			 *  keys have not been mapped,
			 *  and not an ESC sequence, not in insert mode or
			 *	p_ek is on,
			 *  and when not timed out,
			 */
			if ((no_mapping == 0 || allow_keys != 0)
				&& (typebuf.tb_maplen == 0
				    || (p_remap && typebuf.tb_noremap[
						   typebuf.tb_off] == RM_YES))
				&& !timedout)
			{
			    keylen = check_termcode(max_mlen + 1,
							       NULL, 0, NULL);

			    /* If no termcode matched but 'pastetoggle'
			     * matched partially it's like an incomplete key
			     * sequence. */
			    if (keylen == 0 && save_keylen == KEYLEN_PART_KEY)
				keylen = KEYLEN_PART_KEY;

			    /*
			     * When getting a partial match, but the last
			     * characters were not typed, don't wait for a
			     * typed character to complete the termcode.
			     * This helps a lot when a ":normal" command ends
			     * in an ESC.
			     */
			    if (keylen < 0
				       && typebuf.tb_len == typebuf.tb_maplen)
				keylen = 0;
			}
			else
			    keylen = 0;
			if (keylen == 0)	/* no matching terminal code */
			{
#ifdef AMIGA			/* check for window bounds report */
			    if (typebuf.tb_maplen == 0 && (typebuf.tb_buf[
					       typebuf.tb_off] & 0xff) == CSI)
			    {
				for (s = typebuf.tb_buf + typebuf.tb_off + 1;
					s < typebuf.tb_buf + typebuf.tb_off
							      + typebuf.tb_len
				   && (VIM_ISDIGIT(*s) || *s == ';'
								|| *s == ' ');
					++s)
				    ;
				if (*s == 'r' || *s == '|') /* found one */
				{
				    del_typebuf((int)(s + 1 -
				       (typebuf.tb_buf + typebuf.tb_off)), 0);
				    /* get size and redraw screen */
				    shell_resized();
				    continue;
				}
				if (*s == NUL)	    /* need more characters */
				    keylen = KEYLEN_PART_KEY;
			    }
			    if (keylen >= 0)
#endif
			      /* When there was a matching mapping and no
			       * termcode could be replaced after another one,
			       * use that mapping (loop around). If there was
			       * no mapping use the character from the
			       * typeahead buffer right here. */
			      if (mp == NULL)
			      {
/*
 * get a character: 2. from the typeahead buffer
 */
				c = typebuf.tb_buf[typebuf.tb_off] & 255;
				if (advance)	/* remove chars from tb_buf */
				{
				    cmd_silent = (typebuf.tb_silent > 0);
				    if (typebuf.tb_maplen > 0)
					KeyTyped = FALSE;
				    else
				    {
					KeyTyped = TRUE;
					/* write char to script file(s) */
					gotchars(typebuf.tb_buf
							 + typebuf.tb_off, 1);
				    }
				    KeyNoremap = typebuf.tb_noremap[
							      typebuf.tb_off];
				    del_typebuf(1, 0);
				}
				break;	    /* got character, break for loop */
			      }
			}
			if (keylen > 0)	    /* full matching terminal code */
			{
#if defined(FEAT_GUI) && defined(FEAT_MENU)
			    if (typebuf.tb_len >= 2
				&& typebuf.tb_buf[typebuf.tb_off] == K_SPECIAL
					 && typebuf.tb_buf[typebuf.tb_off + 1]
								   == KS_MENU)
			    {
				/*
				 * Using a menu may cause a break in undo!
				 * It's like using gotchars(), but without
				 * recording or writing to a script file.
				 */
				may_sync_undo();
				del_typebuf(3, 0);
				idx = get_menu_index(current_menu, local_State);
				if (idx != MENU_INDEX_INVALID)
				{
				    /*
				     * In Select mode and a Visual mode menu
				     * is used:  Switch to Visual mode
				     * temporarily.  Append K_SELECT to switch
				     * back to Select mode.
				     */
				    if (VIsual_active && VIsual_select
					    && (current_menu->modes & VISUAL))
				    {
					VIsual_select = FALSE;
					(void)ins_typebuf(K_SELECT_STRING,
						  REMAP_NONE, 0, TRUE, FALSE);
				    }
				    ins_typebuf(current_menu->strings[idx],
						current_menu->noremap[idx],
						0, TRUE,
						   current_menu->silent[idx]);
				}
			    }
#endif /* FEAT_GUI && FEAT_MENU */
			    continue;	/* try mapping again */
			}

			/* Partial match: get some more characters.  When a
			 * matching mapping was found use that one. */
			if (mp == NULL || keylen < 0)
			    keylen = KEYLEN_PART_KEY;
			else
			    keylen = mp_match_len;
		    }

		    /* complete match */
		    if (keylen >= 0 && keylen <= typebuf.tb_len)
		    {
#ifdef FEAT_EVAL
			int save_m_expr;
			int save_m_noremap;
			int save_m_silent;
			char_u *save_m_keys;
			char_u *save_m_str;
#else
# define save_m_noremap mp->m_noremap
# define save_m_silent mp->m_silent
#endif

			/* write chars to script file(s) */
			if (keylen > typebuf.tb_maplen)
			    gotchars(typebuf.tb_buf + typebuf.tb_off
							  + typebuf.tb_maplen,
						  keylen - typebuf.tb_maplen);

			cmd_silent = (typebuf.tb_silent > 0);
			del_typebuf(keylen, 0);	/* remove the mapped keys */

			/*
			 * Put the replacement string in front of mapstr.
			 * The depth check catches ":map x y" and ":map y x".
			 */
			if (++mapdepth >= p_mmd)
			{
			    emsg(_("E223: recursive mapping"));
			    if (State & CMDLINE)
				redrawcmdline();
			    else
				setcursor();
			    flush_buffers(FLUSH_MINIMAL);
			    mapdepth = 0;	/* for next one */
			    c = -1;
			    break;
			}

			/*
			 * In Select mode and a Visual mode mapping is used:
			 * Switch to Visual mode temporarily.  Append K_SELECT
			 * to switch back to Select mode.
			 */
			if (VIsual_active && VIsual_select
						     && (mp->m_mode & VISUAL))
			{
			    VIsual_select = FALSE;
			    (void)ins_typebuf(K_SELECT_STRING, REMAP_NONE,
							      0, TRUE, FALSE);
			}

#ifdef FEAT_EVAL
			/* Copy the values from *mp that are used, because
			 * evaluating the expression may invoke a function
			 * that redefines the mapping, thereby making *mp
			 * invalid. */
			save_m_expr = mp->m_expr;
			save_m_noremap = mp->m_noremap;
			save_m_silent = mp->m_silent;
			save_m_keys = NULL;  /* only saved when needed */
			save_m_str = NULL;  /* only saved when needed */

			/*
			 * Handle ":map <expr>": evaluate the {rhs} as an
			 * expression.  Also save and restore the command line
			 * for "normal :".
			 */
			if (mp->m_expr)
			{
			    int		save_vgetc_busy = vgetc_busy;

			    vgetc_busy = 0;
			    save_m_keys = vim_strsave(mp->m_keys);
			    save_m_str = vim_strsave(mp->m_str);
			    s = eval_map_expr(save_m_str, NUL);
			    vgetc_busy = save_vgetc_busy;
			}
			else
#endif
			    s = mp->m_str;

			/*
			 * Insert the 'to' part in the typebuf.tb_buf.
			 * If 'from' field is the same as the start of the
			 * 'to' field, don't remap the first character (but do
			 * allow abbreviations).
			 * If m_noremap is set, don't remap the whole 'to'
			 * part.
			 */
			if (s == NULL)
			    i = FAIL;
			else
			{
			    int noremap;

			    if (save_m_noremap != REMAP_YES)
				noremap = save_m_noremap;
			    else if (
#ifdef FEAT_EVAL
				STRNCMP(s, save_m_keys != NULL
						   ? save_m_keys : mp->m_keys,
							 (size_t)keylen)
#else
				STRNCMP(s, mp->m_keys, (size_t)keylen)
#endif
				   != 0)
				noremap = REMAP_YES;
			    else
				noremap = REMAP_SKIP;
			    i = ins_typebuf(s, noremap,
					0, TRUE, cmd_silent || save_m_silent);
#ifdef FEAT_EVAL
			    if (save_m_expr)
				vim_free(s);
#endif
			}
#ifdef FEAT_EVAL
			vim_free(save_m_keys);
			vim_free(save_m_str);
#endif
			if (i == FAIL)
			{
			    c = -1;
			    break;
			}
			continue;
		    }
		}

/*
 * get a character: 3. from the user - handle <Esc> in Insert mode
 */
		/*
		 * Special case: if we get an <ESC> in insert mode and there
		 * are no more characters at once, we pretend to go out of
		 * insert mode.  This prevents the one second delay after
		 * typing an <ESC>.  If we get something after all, we may
		 * have to redisplay the mode. That the cursor is in the wrong
		 * place does not matter.
		 */
		c = 0;
#ifdef FEAT_CMDL_INFO
		new_wcol = curwin->w_wcol;
		new_wrow = curwin->w_wrow;
#endif
		if (	   advance
			&& typebuf.tb_len == 1
			&& typebuf.tb_buf[typebuf.tb_off] == ESC
			&& !no_mapping
			&& ex_normal_busy == 0
			&& typebuf.tb_maplen == 0
			&& (State & INSERT)
			&& (p_timeout
			    || (keylen == KEYLEN_PART_KEY && p_ttimeout))
			&& (c = inchar(typebuf.tb_buf + typebuf.tb_off
					       + typebuf.tb_len, 3, 25L)) == 0)
		{
		    colnr_T	col = 0, vcol;
		    char_u	*ptr;

		    if (mode_displayed)
		    {
			unshowmode(TRUE);
			mode_deleted = TRUE;
		    }
#ifdef FEAT_GUI
		    /* may show a different cursor shape */
		    if (gui.in_use && State != NORMAL && !cmd_silent)
		    {
			int	    save_State;

			save_State = State;
			State = NORMAL;
			gui_update_cursor(TRUE, FALSE);
			State = save_State;
			shape_changed = TRUE;
		    }
#endif
		    validate_cursor();
		    old_wcol = curwin->w_wcol;
		    old_wrow = curwin->w_wrow;

		    /* move cursor left, if possible */
		    if (curwin->w_cursor.col != 0)
		    {
			if (curwin->w_wcol > 0)
			{
			    if (did_ai)
			    {
				/*
				 * We are expecting to truncate the trailing
				 * white-space, so find the last non-white
				 * character -- webb
				 */
				col = vcol = curwin->w_wcol = 0;
				ptr = ml_get_curline();
				while (col < curwin->w_cursor.col)
				{
				    if (!VIM_ISWHITE(ptr[col]))
					curwin->w_wcol = vcol;
				    vcol += lbr_chartabsize(ptr, ptr + col,
							       (colnr_T)vcol);
				    if (has_mbyte)
					col += (*mb_ptr2len)(ptr + col);
				    else
					++col;
				}
				curwin->w_wrow = curwin->w_cline_row
					   + curwin->w_wcol / curwin->w_width;
				curwin->w_wcol %= curwin->w_width;
				curwin->w_wcol += curwin_col_off();
				col = 0;	/* no correction needed */
			    }
			    else
			    {
				--curwin->w_wcol;
				col = curwin->w_cursor.col - 1;
			    }
			}
			else if (curwin->w_p_wrap && curwin->w_wrow)
			{
			    --curwin->w_wrow;
			    curwin->w_wcol = curwin->w_width - 1;
			    col = curwin->w_cursor.col - 1;
			}
			if (has_mbyte && col > 0 && curwin->w_wcol > 0)
			{
			    /* Correct when the cursor is on the right halve
			     * of a double-wide character. */
			    ptr = ml_get_curline();
			    col -= (*mb_head_off)(ptr, ptr + col);
			    if ((*mb_ptr2cells)(ptr + col) > 1)
				--curwin->w_wcol;
			}
		    }
		    setcursor();
		    out_flush();
#ifdef FEAT_CMDL_INFO
		    new_wcol = curwin->w_wcol;
		    new_wrow = curwin->w_wrow;
#endif
		    curwin->w_wcol = old_wcol;
		    curwin->w_wrow = old_wrow;
		}
		if (c < 0)
		    continue;	/* end of input script reached */

		/* Allow mapping for just typed characters. When we get here c
		 * is the number of extra bytes and typebuf.tb_len is 1. */
		for (n = 1; n <= c; ++n)
		    typebuf.tb_noremap[typebuf.tb_off + n] = RM_YES;
		typebuf.tb_len += c;

		/* buffer full, don't map */
		if (typebuf.tb_len >= typebuf.tb_maplen + MAXMAPLEN)
		{
		    timedout = TRUE;
		    continue;
		}

		if (ex_normal_busy > 0)
		{
#ifdef FEAT_CMDWIN
		    static int tc = 0;
#endif

		    /* No typeahead left and inside ":normal".  Must return
		     * something to avoid getting stuck.  When an incomplete
		     * mapping is present, behave like it timed out. */
		    if (typebuf.tb_len > 0)
		    {
			timedout = TRUE;
			continue;
		    }
		    /* When 'insertmode' is set, ESC just beeps in Insert
		     * mode.  Use CTRL-L to make edit() return.
		     * For the command line only CTRL-C always breaks it.
		     * For the cmdline window: Alternate between ESC and
		     * CTRL-C: ESC for most situations and CTRL-C to close the
		     * cmdline window. */
		    if (p_im && (State & INSERT))
			c = Ctrl_L;
#ifdef FEAT_TERMINAL
		    else if (terminal_is_active())
			c = K_CANCEL;
#endif
		    else if ((State & CMDLINE)
#ifdef FEAT_CMDWIN
			    || (cmdwin_type > 0 && tc == ESC)
#endif
			    )
			c = Ctrl_C;
		    else
			c = ESC;
#ifdef FEAT_CMDWIN
		    tc = c;
#endif
		    break;
		}

/*
 * get a character: 3. from the user - update display
 */
		/* In insert mode a screen update is skipped when characters
		 * are still available.  But when those available characters
		 * are part of a mapping, and we are going to do a blocking
		 * wait here.  Need to update the screen to display the
		 * changed text so far. Also for when 'lazyredraw' is set and
		 * redrawing was postponed because there was something in the
		 * input buffer (e.g., termresponse). */
		if (((State & INSERT) != 0 || p_lz) && (State & CMDLINE) == 0
			  && advance && must_redraw != 0 && !need_wait_return)
		{
		    update_screen(0);
		    setcursor(); /* put cursor back where it belongs */
		}

		/*
		 * If we have a partial match (and are going to wait for more
		 * input from the user), show the partially matched characters
		 * to the user with showcmd.
		 */
#ifdef FEAT_CMDL_INFO
		i = 0;
#endif
		c1 = 0;
		if (typebuf.tb_len > 0 && advance && !exmode_active)
		{
		    if (((State & (NORMAL | INSERT)) || State == LANGMAP)
			    && State != HITRETURN)
		    {
			/* this looks nice when typing a dead character map */
			if (State & INSERT
			    && ptr2cells(typebuf.tb_buf + typebuf.tb_off
						   + typebuf.tb_len - 1) == 1)
			{
			    edit_putchar(typebuf.tb_buf[typebuf.tb_off
						+ typebuf.tb_len - 1], FALSE);
			    setcursor(); /* put cursor back where it belongs */
			    c1 = 1;
			}
#ifdef FEAT_CMDL_INFO
			/* need to use the col and row from above here */
			old_wcol = curwin->w_wcol;
			old_wrow = curwin->w_wrow;
			curwin->w_wcol = new_wcol;
			curwin->w_wrow = new_wrow;
			push_showcmd();
			if (typebuf.tb_len > SHOWCMD_COLS)
			    i = typebuf.tb_len - SHOWCMD_COLS;
			while (i < typebuf.tb_len)
			    (void)add_to_showcmd(typebuf.tb_buf[typebuf.tb_off
								      + i++]);
			curwin->w_wcol = old_wcol;
			curwin->w_wrow = old_wrow;
#endif
		    }

		    /* this looks nice when typing a dead character map */
		    if ((State & CMDLINE)
#if defined(FEAT_CRYPT) || defined(FEAT_EVAL)
			    && cmdline_star == 0
#endif
			    && ptr2cells(typebuf.tb_buf + typebuf.tb_off
						   + typebuf.tb_len - 1) == 1)
		    {
			putcmdline(typebuf.tb_buf[typebuf.tb_off
						+ typebuf.tb_len - 1], FALSE);
			c1 = 1;
		    }
		}

/*
 * get a character: 3. from the user - get it
 */
		if (typebuf.tb_len == 0)
		    // timedout may have been set while waiting for a mapping
		    // that has a <Nop> RHS.
		    timedout = FALSE;

		if (advance)
		{
		    if (typebuf.tb_len == 0
			    || !(p_timeout
				 || (p_ttimeout && keylen == KEYLEN_PART_KEY)))
			// blocking wait
			wait_time = -1L;
		    else if (keylen == KEYLEN_PART_KEY && p_ttm >= 0)
			wait_time = p_ttm;
		    else
			wait_time = p_tm;
		}
		else
		    wait_time = 0;

		wait_tb_len = typebuf.tb_len;
		c = inchar(typebuf.tb_buf + typebuf.tb_off + typebuf.tb_len,
			typebuf.tb_buflen - typebuf.tb_off - typebuf.tb_len - 1,
			wait_time);

#ifdef FEAT_CMDL_INFO
		if (i != 0)
		    pop_showcmd();
#endif
		if (c1 == 1)
		{
		    if (State & INSERT)
			edit_unputchar();
		    if (State & CMDLINE)
			unputcmdline();
		    else
			setcursor();	/* put cursor back where it belongs */
		}

		if (c < 0)
		    continue;		/* end of input script reached */
		if (c == NUL)		/* no character available */
		{
		    if (!advance)
			break;
		    if (wait_tb_len > 0)	/* timed out */
		    {
			timedout = TRUE;
			continue;
		    }
		}
		else
		{	    /* allow mapping for just typed characters */
		    while (typebuf.tb_buf[typebuf.tb_off
						     + typebuf.tb_len] != NUL)
			typebuf.tb_noremap[typebuf.tb_off
						 + typebuf.tb_len++] = RM_YES;
#ifdef HAVE_INPUT_METHOD
		    /* Get IM status right after getting keys, not after the
		     * timeout for a mapping (focus may be lost by then). */
		    vgetc_im_active = im_get_status();
#endif
		}
	    }	    /* for (;;) */
	}	/* if (!character from stuffbuf) */

	/* if advance is FALSE don't loop on NULs */
    } while ((c < 0 && c != K_CANCEL) || (advance && c == NUL));

    /*
     * The "INSERT" message is taken care of here:
     *	 if we return an ESC to exit insert mode, the message is deleted
     *	 if we don't return an ESC but deleted the message before, redisplay it
     */
    if (advance && p_smd && msg_silent == 0 && (State & INSERT))
    {
	if (c == ESC && !mode_deleted && !no_mapping && mode_displayed)
	{
	    if (typebuf.tb_len && !KeyTyped)
		redraw_cmdline = TRUE;	    /* delete mode later */
	    else
		unshowmode(FALSE);
	}
	else if (c != ESC && mode_deleted)
	{
	    if (typebuf.tb_len && !KeyTyped)
		redraw_cmdline = TRUE;	    /* show mode later */
	    else
		showmode();
	}
    }
#ifdef FEAT_GUI
    /* may unshow different cursor shape */
    if (gui.in_use && shape_changed)
	gui_update_cursor(TRUE, FALSE);
#endif
    if (timedout && c == ESC)
    {
	char_u nop_buf[3];

	// When recording there will be no timeout.  Add a <Nop> after the ESC
	// to avoid that it forms a key code with following characters.
	nop_buf[0] = K_SPECIAL;
	nop_buf[1] = KS_EXTRA;
	nop_buf[2] = KE_NOP;
	gotchars(nop_buf, 3);
    }

    --vgetc_busy;

    return c;
}