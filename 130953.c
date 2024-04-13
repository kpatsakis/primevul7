do_ecmd(
    int		fnum,
    char_u	*ffname,
    char_u	*sfname,
    exarg_T	*eap,			// can be NULL!
    linenr_T	newlnum,
    int		flags,
    win_T	*oldwin)
{
    int		other_file;		// TRUE if editing another file
    int		oldbuf;			// TRUE if using existing buffer
    int		auto_buf = FALSE;	// TRUE if autocommands brought us
					// into the buffer unexpectedly
    char_u	*new_name = NULL;
#if defined(FEAT_EVAL)
    int		did_set_swapcommand = FALSE;
#endif
    buf_T	*buf;
    bufref_T	bufref;
    bufref_T	old_curbuf;
    char_u	*free_fname = NULL;
#ifdef FEAT_BROWSE
    char_u	dot_path[] = ".";
    char_u	*browse_file = NULL;
#endif
    int		retval = FAIL;
    long	n;
    pos_T	orig_pos;
    linenr_T	topline = 0;
    int		newcol = -1;
    int		solcol = -1;
    pos_T	*pos;
    char_u	*command = NULL;
#ifdef FEAT_SPELL
    int		did_get_winopts = FALSE;
#endif
    int		readfile_flags = 0;
    int		did_inc_redrawing_disabled = FALSE;
    long	*so_ptr = curwin->w_p_so >= 0 ? &curwin->w_p_so : &p_so;

#ifdef FEAT_PROP_POPUP
    if (ERROR_IF_TERM_POPUP_WINDOW)
	return FAIL;
#endif

    if (eap != NULL)
	command = eap->do_ecmd_cmd;
    set_bufref(&old_curbuf, curbuf);

    if (fnum != 0)
    {
	if (fnum == curbuf->b_fnum)	// file is already being edited
	    return OK;			// nothing to do
	other_file = TRUE;
    }
    else
    {
#ifdef FEAT_BROWSE
	if ((cmdmod.cmod_flags & CMOD_BROWSE) && !exiting)
	{
	    if (
# ifdef FEAT_GUI
		!gui.in_use &&
# endif
		    au_has_group((char_u *)"FileExplorer"))
	    {
		// No browsing supported but we do have the file explorer:
		// Edit the directory.
		if (ffname == NULL || !mch_isdir(ffname))
		    ffname = dot_path;
	    }
	    else
	    {
		browse_file = do_browse(0, (char_u *)_("Edit File"), ffname,
						    NULL, NULL, NULL, curbuf);
		if (browse_file == NULL)
		    goto theend;
		ffname = browse_file;
	    }
	}
#endif
	// if no short name given, use ffname for short name
	if (sfname == NULL)
	    sfname = ffname;
#ifdef USE_FNAME_CASE
	if (sfname != NULL)
	    fname_case(sfname, 0);   // set correct case for sfname
#endif

	if ((flags & (ECMD_ADDBUF | ECMD_ALTBUF))
					 && (ffname == NULL || *ffname == NUL))
	    goto theend;

	if (ffname == NULL)
	    other_file = TRUE;
					    // there is no file name
	else if (*ffname == NUL && curbuf->b_ffname == NULL)
	    other_file = FALSE;
	else
	{
	    if (*ffname == NUL)		    // re-edit with same file name
	    {
		ffname = curbuf->b_ffname;
		sfname = curbuf->b_fname;
	    }
	    free_fname = fix_fname(ffname); // may expand to full path name
	    if (free_fname != NULL)
		ffname = free_fname;
	    other_file = otherfile(ffname);
	}
    }

    /*
     * If the file was changed we may not be allowed to abandon it:
     * - if we are going to re-edit the same file
     * - or if we are the only window on this file and if ECMD_HIDE is FALSE
     */
    if (  ((!other_file && !(flags & ECMD_OLDBUF))
	    || (curbuf->b_nwindows == 1
		&& !(flags & (ECMD_HIDE | ECMD_ADDBUF | ECMD_ALTBUF))))
	&& check_changed(curbuf, (p_awa ? CCGD_AW : 0)
			       | (other_file ? 0 : CCGD_MULTWIN)
			       | ((flags & ECMD_FORCEIT) ? CCGD_FORCEIT : 0)
			       | (eap == NULL ? 0 : CCGD_EXCMD)))
    {
	if (fnum == 0 && other_file && ffname != NULL)
	    (void)setaltfname(ffname, sfname, newlnum < 0 ? 0 : newlnum);
	goto theend;
    }

    /*
     * End Visual mode before switching to another buffer, so the text can be
     * copied into the GUI selection buffer.
     */
    reset_VIsual();

#if defined(FEAT_EVAL)
    if ((command != NULL || newlnum > (linenr_T)0)
	    && *get_vim_var_str(VV_SWAPCOMMAND) == NUL)
    {
	int	len;
	char_u	*p;

	// Set v:swapcommand for the SwapExists autocommands.
	if (command != NULL)
	    len = (int)STRLEN(command) + 3;
	else
	    len = 30;
	p = alloc(len);
	if (p != NULL)
	{
	    if (command != NULL)
		vim_snprintf((char *)p, len, ":%s\r", command);
	    else
		vim_snprintf((char *)p, len, "%ldG", (long)newlnum);
	    set_vim_var_string(VV_SWAPCOMMAND, p, -1);
	    did_set_swapcommand = TRUE;
	    vim_free(p);
	}
    }
#endif

    /*
     * If we are starting to edit another file, open a (new) buffer.
     * Otherwise we re-use the current buffer.
     */
    if (other_file)
    {
	int prev_alt_fnum = curwin->w_alt_fnum;

	if (!(flags & (ECMD_ADDBUF | ECMD_ALTBUF)))
	{
	    if ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
		curwin->w_alt_fnum = curbuf->b_fnum;
	    if (oldwin != NULL)
		buflist_altfpos(oldwin);
	}

	if (fnum)
	    buf = buflist_findnr(fnum);
	else
	{
	    if (flags & (ECMD_ADDBUF | ECMD_ALTBUF))
	    {
		// Default the line number to zero to avoid that a wininfo item
		// is added for the current window.
		linenr_T	tlnum = 0;
		buf_T		*newbuf;

		if (command != NULL)
		{
		    tlnum = atol((char *)command);
		    if (tlnum <= 0)
			tlnum = 1L;
		}
		// Add BLN_NOCURWIN to avoid a new wininfo items are associated
		// with the current window.
		newbuf = buflist_new(ffname, sfname, tlnum,
						    BLN_LISTED | BLN_NOCURWIN);
		if (newbuf != NULL && (flags & ECMD_ALTBUF))
		    curwin->w_alt_fnum = newbuf->b_fnum;
		goto theend;
	    }
	    buf = buflist_new(ffname, sfname, 0L,
		    BLN_CURBUF | ((flags & ECMD_SET_HELP) ? 0 : BLN_LISTED));

	    // autocommands may change curwin and curbuf
	    if (oldwin != NULL)
		oldwin = curwin;
	    set_bufref(&old_curbuf, curbuf);
	}
	if (buf == NULL)
	    goto theend;
	if (curwin->w_alt_fnum == buf->b_fnum && prev_alt_fnum != 0)
	    // reusing the buffer, keep the old alternate file
	    curwin->w_alt_fnum = prev_alt_fnum;

	if (buf->b_ml.ml_mfp == NULL)		// no memfile yet
	{
	    oldbuf = FALSE;
	}
	else					// existing memfile
	{
	    oldbuf = TRUE;
	    set_bufref(&bufref, buf);
	    (void)buf_check_timestamp(buf, FALSE);
	    // Check if autocommands made the buffer invalid or changed the
	    // current buffer.
	    if (!bufref_valid(&bufref) || curbuf != old_curbuf.br_buf)
		goto theend;
#ifdef FEAT_EVAL
	    if (aborting())	    // autocmds may abort script processing
		goto theend;
#endif
	}

	// May jump to last used line number for a loaded buffer or when asked
	// for explicitly
	if ((oldbuf && newlnum == ECMD_LASTL) || newlnum == ECMD_LAST)
	{
	    pos = buflist_findfpos(buf);
	    newlnum = pos->lnum;
	    solcol = pos->col;
	}

	/*
	 * Make the (new) buffer the one used by the current window.
	 * If the old buffer becomes unused, free it if ECMD_HIDE is FALSE.
	 * If the current buffer was empty and has no file name, curbuf
	 * is returned by buflist_new(), nothing to do here.
	 */
	if (buf != curbuf)
	{
	    bufref_T	save_au_new_curbuf;
#ifdef FEAT_CMDWIN
	    int		save_cmdwin_type = cmdwin_type;

	    // BufLeave applies to the old buffer.
	    cmdwin_type = 0;
#endif
	    /*
	     * Be careful: The autocommands may delete any buffer and change
	     * the current buffer.
	     * - If the buffer we are going to edit is deleted, give up.
	     * - If the current buffer is deleted, prefer to load the new
	     *   buffer when loading a buffer is required.  This avoids
	     *   loading another buffer which then must be closed again.
	     * - If we ended up in the new buffer already, need to skip a few
	     *	 things, set auto_buf.
	     */
	    if (buf->b_fname != NULL)
		new_name = vim_strsave(buf->b_fname);
	    save_au_new_curbuf = au_new_curbuf;
	    set_bufref(&au_new_curbuf, buf);
	    apply_autocmds(EVENT_BUFLEAVE, NULL, NULL, FALSE, curbuf);
#ifdef FEAT_CMDWIN
	    cmdwin_type = save_cmdwin_type;
#endif
	    if (!bufref_valid(&au_new_curbuf))
	    {
		// new buffer has been deleted
		delbuf_msg(new_name);	// frees new_name
		au_new_curbuf = save_au_new_curbuf;
		goto theend;
	    }
#ifdef FEAT_EVAL
	    if (aborting())	    // autocmds may abort script processing
	    {
		vim_free(new_name);
		au_new_curbuf = save_au_new_curbuf;
		goto theend;
	    }
#endif
	    if (buf == curbuf)		// already in new buffer
		auto_buf = TRUE;
	    else
	    {
		win_T	    *the_curwin = curwin;
		int	    did_decrement;
		buf_T	    *was_curbuf = curbuf;

		// Set the w_closing flag to avoid that autocommands close the
		// window.  And set b_locked for the same reason.
		the_curwin->w_closing = TRUE;
		++buf->b_locked;

		if (curbuf == old_curbuf.br_buf)
		    buf_copy_options(buf, BCO_ENTER);

		// Close the link to the current buffer. This will set
		// oldwin->w_buffer to NULL.
		u_sync(FALSE);
		did_decrement = close_buffer(oldwin, curbuf,
			 (flags & ECMD_HIDE) ? 0 : DOBUF_UNLOAD, FALSE, FALSE);

		// Autocommands may have closed the window.
		if (win_valid(the_curwin))
		    the_curwin->w_closing = FALSE;
		--buf->b_locked;

#ifdef FEAT_EVAL
		// autocmds may abort script processing
		if (aborting() && curwin->w_buffer != NULL)
		{
		    vim_free(new_name);
		    au_new_curbuf = save_au_new_curbuf;
		    goto theend;
		}
#endif
		// Be careful again, like above.
		if (!bufref_valid(&au_new_curbuf))
		{
		    // new buffer has been deleted
		    delbuf_msg(new_name);	// frees new_name
		    au_new_curbuf = save_au_new_curbuf;
		    goto theend;
		}
		if (buf == curbuf)		// already in new buffer
		{
		    // close_buffer() has decremented the window count,
		    // increment it again here and restore w_buffer.
		    if (did_decrement && buf_valid(was_curbuf))
			++was_curbuf->b_nwindows;
		    if (win_valid_any_tab(oldwin) && oldwin->w_buffer == NULL)
			oldwin->w_buffer = was_curbuf;
		    auto_buf = TRUE;
		}
		else
		{
#ifdef FEAT_SYN_HL
		    /*
		     * <VN> We could instead free the synblock
		     * and re-attach to buffer, perhaps.
		     */
		    if (curwin->w_buffer == NULL
			    || curwin->w_s == &(curwin->w_buffer->b_s))
			curwin->w_s = &(buf->b_s);
#endif
		    curwin->w_buffer = buf;
		    curbuf = buf;
		    ++curbuf->b_nwindows;

		    // Set 'fileformat', 'binary' and 'fenc' when forced.
		    if (!oldbuf && eap != NULL)
		    {
			set_file_options(TRUE, eap);
			set_forced_fenc(eap);
		    }
		}

		// May get the window options from the last time this buffer
		// was in this window (or another window).  If not used
		// before, reset the local window options to the global
		// values.  Also restores old folding stuff.
		get_winopts(curbuf);
#ifdef FEAT_SPELL
		did_get_winopts = TRUE;
#endif
	    }
	    vim_free(new_name);
	    au_new_curbuf = save_au_new_curbuf;
	}

	curwin->w_pcmark.lnum = 1;
	curwin->w_pcmark.col = 0;
    }
    else // !other_file
    {
	if ((flags & (ECMD_ADDBUF | ECMD_ALTBUF)) || check_fname() == FAIL)
	    goto theend;

	oldbuf = (flags & ECMD_OLDBUF);
    }

    // Don't redraw until the cursor is in the right line, otherwise
    // autocommands may cause ml_get errors.
    ++RedrawingDisabled;
    did_inc_redrawing_disabled = TRUE;

    buf = curbuf;
    if ((flags & ECMD_SET_HELP) || keep_help_flag)
    {
	prepare_help_buffer();
    }
    else
    {
	// Don't make a buffer listed if it's a help buffer.  Useful when
	// using CTRL-O to go back to a help file.
	if (!curbuf->b_help)
	    set_buflisted(TRUE);
    }

    // If autocommands change buffers under our fingers, forget about
    // editing the file.
    if (buf != curbuf)
	goto theend;
#ifdef FEAT_EVAL
    if (aborting())	    // autocmds may abort script processing
	goto theend;
#endif

    // Since we are starting to edit a file, consider the filetype to be
    // unset.  Helps for when an autocommand changes files and expects syntax
    // highlighting to work in the other file.
    did_filetype = FALSE;

/*
 * other_file	oldbuf
 *  FALSE	FALSE	    re-edit same file, buffer is re-used
 *  FALSE	TRUE	    re-edit same file, nothing changes
 *  TRUE	FALSE	    start editing new file, new buffer
 *  TRUE	TRUE	    start editing in existing buffer (nothing to do)
 */
    if (!other_file && !oldbuf)		// re-use the buffer
    {
	set_last_cursor(curwin);	// may set b_last_cursor
	if (newlnum == ECMD_LAST || newlnum == ECMD_LASTL)
	{
	    newlnum = curwin->w_cursor.lnum;
	    solcol = curwin->w_cursor.col;
	}
	buf = curbuf;
	if (buf->b_fname != NULL)
	    new_name = vim_strsave(buf->b_fname);
	else
	    new_name = NULL;
	set_bufref(&bufref, buf);

	// If the buffer was used before, store the current contents so that
	// the reload can be undone.  Do not do this if the (empty) buffer is
	// being re-used for another file.
	if (!(curbuf->b_flags & BF_NEVERLOADED)
		&& (p_ur < 0 || curbuf->b_ml.ml_line_count <= p_ur))
	{
	    // Sync first so that this is a separate undo-able action.
	    u_sync(FALSE);
	    if (u_savecommon(0, curbuf->b_ml.ml_line_count + 1, 0, TRUE)
								     == FAIL)
	    {
		vim_free(new_name);
		goto theend;
	    }
	    u_unchanged(curbuf);
	    buf_freeall(curbuf, BFA_KEEP_UNDO);

	    // tell readfile() not to clear or reload undo info
	    readfile_flags = READ_KEEP_UNDO;
	}
	else
	    buf_freeall(curbuf, 0);   // free all things for buffer

	// If autocommands deleted the buffer we were going to re-edit, give
	// up and jump to the end.
	if (!bufref_valid(&bufref))
	{
	    delbuf_msg(new_name);	// frees new_name
	    goto theend;
	}
	vim_free(new_name);

	// If autocommands change buffers under our fingers, forget about
	// re-editing the file.  Should do the buf_clear_file(), but perhaps
	// the autocommands changed the buffer...
	if (buf != curbuf)
	    goto theend;
#ifdef FEAT_EVAL
	if (aborting())	    // autocmds may abort script processing
	    goto theend;
#endif
	buf_clear_file(curbuf);
	curbuf->b_op_start.lnum = 0;	// clear '[ and '] marks
	curbuf->b_op_end.lnum = 0;
    }

/*
 * If we get here we are sure to start editing
 */
    // Assume success now
    retval = OK;

    /*
     * Check if we are editing the w_arg_idx file in the argument list.
     */
    check_arg_idx(curwin);

    if (!auto_buf)
    {
	/*
	 * Set cursor and init window before reading the file and executing
	 * autocommands.  This allows for the autocommands to position the
	 * cursor.
	 */
	curwin_init();

#ifdef FEAT_FOLDING
	// It's possible that all lines in the buffer changed.  Need to update
	// automatic folding for all windows where it's used.
	{
	    win_T	    *win;
	    tabpage_T	    *tp;

	    FOR_ALL_TAB_WINDOWS(tp, win)
		if (win->w_buffer == curbuf)
		    foldUpdateAll(win);
	}
#endif

	// Change directories when the 'acd' option is set.
	DO_AUTOCHDIR;

	/*
	 * Careful: open_buffer() and apply_autocmds() may change the current
	 * buffer and window.
	 */
	orig_pos = curwin->w_cursor;
	topline = curwin->w_topline;
	if (!oldbuf)			    // need to read the file
	{
#ifdef FEAT_PROP_POPUP
	    // Don't use the swap-exists dialog for a popup window, can't edit
	    // the buffer.
	    if (WIN_IS_POPUP(curwin))
		curbuf->b_flags |= BF_NO_SEA;
#endif
	    swap_exists_action = SEA_DIALOG;
	    curbuf->b_flags |= BF_CHECK_RO; // set/reset 'ro' flag

	    /*
	     * Open the buffer and read the file.
	     */
	    if (flags & ECMD_NOWINENTER)
		readfile_flags |= READ_NOWINENTER;
#if defined(FEAT_EVAL)
	    if (should_abort(open_buffer(FALSE, eap, readfile_flags)))
		retval = FAIL;
#else
	    (void)open_buffer(FALSE, eap, readfile_flags);
#endif

#ifdef FEAT_PROP_POPUP
	    curbuf->b_flags &= ~BF_NO_SEA;
#endif
	    if (swap_exists_action == SEA_QUIT)
		retval = FAIL;
	    handle_swap_exists(&old_curbuf);
	}
	else
	{
	    // Read the modelines, but only to set window-local options.  Any
	    // buffer-local options have already been set and may have been
	    // changed by the user.
	    do_modelines(OPT_WINONLY);

	    apply_autocmds_retval(EVENT_BUFENTER, NULL, NULL, FALSE,
							      curbuf, &retval);
	    if ((flags & ECMD_NOWINENTER) == 0)
		apply_autocmds_retval(EVENT_BUFWINENTER, NULL, NULL, FALSE,
							      curbuf, &retval);
	}
	check_arg_idx(curwin);

	// If autocommands change the cursor position or topline, we should
	// keep it.  Also when it moves within a line. But not when it moves
	// to the first non-blank.
	if (!EQUAL_POS(curwin->w_cursor, orig_pos))
	{
	    char_u *text = ml_get_curline();

	    if (curwin->w_cursor.lnum != orig_pos.lnum
		    || curwin->w_cursor.col != (int)(skipwhite(text) - text))
	    {
		newlnum = curwin->w_cursor.lnum;
		newcol = curwin->w_cursor.col;
	    }
	}
	if (curwin->w_topline == topline)
	    topline = 0;

	// Even when cursor didn't move we need to recompute topline.
	changed_line_abv_curs();

	maketitle();
#if defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	if (WIN_IS_POPUP(curwin) && curwin->w_p_pvw && retval != FAIL)
	    popup_set_title(curwin);
#endif
    }

#ifdef FEAT_DIFF
    // Tell the diff stuff that this buffer is new and/or needs updating.
    // Also needed when re-editing the same buffer, because unloading will
    // have removed it as a diff buffer.
    if (curwin->w_p_diff)
    {
	diff_buf_add(curbuf);
	diff_invalidate(curbuf);
    }
#endif

#ifdef FEAT_SPELL
    // If the window options were changed may need to set the spell language.
    // Can only do this after the buffer has been properly setup.
    if (did_get_winopts && curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	(void)did_set_spelllang(curwin);
#endif

    if (command == NULL)
    {
	if (newcol >= 0)	// position set by autocommands
	{
	    curwin->w_cursor.lnum = newlnum;
	    curwin->w_cursor.col = newcol;
	    check_cursor();
	}
	else if (newlnum > 0)	// line number from caller or old position
	{
	    curwin->w_cursor.lnum = newlnum;
	    check_cursor_lnum();
	    if (solcol >= 0 && !p_sol)
	    {
		// 'sol' is off: Use last known column.
		curwin->w_cursor.col = solcol;
		check_cursor_col();
		curwin->w_cursor.coladd = 0;
		curwin->w_set_curswant = TRUE;
	    }
	    else
		beginline(BL_SOL | BL_FIX);
	}
	else			// no line number, go to last line in Ex mode
	{
	    if (exmode_active)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
	    beginline(BL_WHITE | BL_FIX);
	}
    }

    // Check if cursors in other windows on the same buffer are still valid
    check_lnums(FALSE);

    /*
     * Did not read the file, need to show some info about the file.
     * Do this after setting the cursor.
     */
    if (oldbuf && !auto_buf)
    {
	int	msg_scroll_save = msg_scroll;

	// Obey the 'O' flag in 'cpoptions': overwrite any previous file
	// message.
	if (shortmess(SHM_OVERALL) && !exiting && p_verbose == 0)
	    msg_scroll = FALSE;
	if (!msg_scroll)	// wait a bit when overwriting an error msg
	    check_for_delay(FALSE);
	msg_start();
	msg_scroll = msg_scroll_save;
	msg_scrolled_ign = TRUE;

	if (!shortmess(SHM_FILEINFO))
	    fileinfo(FALSE, TRUE, FALSE);

	msg_scrolled_ign = FALSE;
    }

#ifdef FEAT_VIMINFO
    curbuf->b_last_used = vim_time();
#endif

    if (command != NULL)
	do_cmdline(command, NULL, NULL, DOCMD_VERBOSE|DOCMD_RANGEOK);

#ifdef FEAT_KEYMAP
    if (curbuf->b_kmap_state & KEYMAP_INIT)
	(void)keymap_init();
#endif

    --RedrawingDisabled;
    did_inc_redrawing_disabled = FALSE;
    if (!skip_redraw)
    {
	n = *so_ptr;
	if (topline == 0 && command == NULL)
	    *so_ptr = 9999;		// force cursor halfway the window
	update_topline();
	curwin->w_scbind_pos = curwin->w_topline;
	*so_ptr = n;
	redraw_curbuf_later(NOT_VALID);	// redraw this buffer later
    }

    if (p_im && (State & MODE_INSERT) == 0)
	need_start_insertmode = TRUE;

#ifdef FEAT_AUTOCHDIR
    // Change directories when the 'acd' option is set and we aren't already in
    // that directory (should already be done above). Expect getcwd() to be
    // faster than calling shorten_fnames() unnecessarily.
    if (p_acd && curbuf->b_ffname != NULL)
    {
	char_u	curdir[MAXPATHL];
	char_u	filedir[MAXPATHL];

	vim_strncpy(filedir, curbuf->b_ffname, MAXPATHL - 1);
	*gettail_sep(filedir) = NUL;
	if (mch_dirname(curdir, MAXPATHL) != FAIL
		&& vim_fnamecmp(curdir, filedir) != 0)
	    do_autochdir();
    }
#endif

#if defined(FEAT_NETBEANS_INTG)
    if (curbuf->b_ffname != NULL)
    {
# ifdef FEAT_NETBEANS_INTG
	if ((flags & ECMD_SET_HELP) != ECMD_SET_HELP)
	    netbeans_file_opened(curbuf);
# endif
    }
#endif

theend:
    if (did_inc_redrawing_disabled)
	--RedrawingDisabled;
#if defined(FEAT_EVAL)
    if (did_set_swapcommand)
	set_vim_var_string(VV_SWAPCOMMAND, NULL, -1);
#endif
#ifdef FEAT_BROWSE
    vim_free(browse_file);
#endif
    vim_free(free_fname);
    return retval;
}