getfile(
    int		fnum,
    char_u	*ffname_arg,
    char_u	*sfname_arg,
    int		setpm,
    linenr_T	lnum,
    int		forceit)
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    int		other;
    int		retval;
    char_u	*free_me = NULL;

    if (text_locked())
	return GETFILE_ERROR;
    if (curbuf_locked())
	return GETFILE_ERROR;

    if (fnum == 0)
    {
					// make ffname full path, set sfname
	fname_expand(curbuf, &ffname, &sfname);
	other = otherfile(ffname);
	free_me = ffname;		// has been allocated, free() later
    }
    else
	other = (fnum != curbuf->b_fnum);

    if (other)
	++no_wait_return;	    // don't wait for autowrite message
    if (other && !forceit && curbuf->b_nwindows == 1 && !buf_hide(curbuf)
		   && curbufIsChanged() && autowrite(curbuf, forceit) == FAIL)
    {
#if defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	if (p_confirm && p_write)
	    dialog_changed(curbuf, FALSE);
	if (curbufIsChanged())
#endif
	{
	    --no_wait_return;
	    no_write_message();
	    retval = GETFILE_NOT_WRITTEN;	// file has been changed
	    goto theend;
	}
    }
    if (other)
	--no_wait_return;
    if (setpm)
	setpcmark();
    if (!other)
    {
	if (lnum != 0)
	    curwin->w_cursor.lnum = lnum;
	check_cursor_lnum();
	beginline(BL_SOL | BL_FIX);
	retval = GETFILE_SAME_FILE;	// it's in the same file
    }
    else if (do_ecmd(fnum, ffname, sfname, NULL, lnum,
	     (buf_hide(curbuf) ? ECMD_HIDE : 0) + (forceit ? ECMD_FORCEIT : 0),
		curwin) == OK)
	retval = GETFILE_OPEN_OTHER;	// opened another file
    else
	retval = GETFILE_ERROR;		// error encountered

theend:
    vim_free(free_me);
    return retval;
}