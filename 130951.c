check_overwrite(
    exarg_T	*eap,
    buf_T	*buf,
    char_u	*fname,	    // file name to be used (can differ from
			    // buf->ffname)
    char_u	*ffname,    // full path version of fname
    int		other)	    // writing under other name
{
    /*
     * Write to another file or b_flags set or not writing the whole file:
     * overwriting only allowed with '!'.
     */
    if (       (other
		|| (buf->b_flags & BF_NOTEDITED)
		|| ((buf->b_flags & BF_NEW)
		    && vim_strchr(p_cpo, CPO_OVERNEW) == NULL)
		|| (buf->b_flags & BF_READERR))
	    && !p_wa
	    && vim_fexists(ffname))
    {
	if (!eap->forceit && !eap->append)
	{
#ifdef UNIX
	    // with UNIX it is possible to open a directory
	    if (mch_isdir(ffname))
	    {
		semsg(_(e_str_is_directory), ffname);
		return FAIL;
	    }
#endif
#if defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    if (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
	    {
		char_u	buff[DIALOG_MSG_SIZE];

		dialog_msg(buff, _("Overwrite existing file \"%s\"?"), fname);
		if (vim_dialog_yesno(VIM_QUESTION, NULL, buff, 2) != VIM_YES)
		    return FAIL;
		eap->forceit = TRUE;
	    }
	    else
#endif
	    {
		emsg(_(e_file_exists));
		return FAIL;
	    }
	}

	// For ":w! filename" check that no swap file exists for "filename".
	if (other && !emsg_silent)
	{
	    char_u	*dir;
	    char_u	*p;
	    int		r;
	    char_u	*swapname;

	    // We only try the first entry in 'directory', without checking if
	    // it's writable.  If the "." directory is not writable the write
	    // will probably fail anyway.
	    // Use 'shortname' of the current buffer, since there is no buffer
	    // for the written file.
	    if (*p_dir == NUL)
	    {
		dir = alloc(5);
		if (dir == NULL)
		    return FAIL;
		STRCPY(dir, ".");
	    }
	    else
	    {
		dir = alloc(MAXPATHL);
		if (dir == NULL)
		    return FAIL;
		p = p_dir;
		copy_option_part(&p, dir, MAXPATHL, ",");
	    }
	    swapname = makeswapname(fname, ffname, curbuf, dir);
	    vim_free(dir);
	    r = vim_fexists(swapname);
	    if (r)
	    {
#if defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
		if (p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM))
		{
		    char_u	buff[DIALOG_MSG_SIZE];

		    dialog_msg(buff,
			    _("Swap file \"%s\" exists, overwrite anyway?"),
								    swapname);
		    if (vim_dialog_yesno(VIM_QUESTION, NULL, buff, 2)
								   != VIM_YES)
		    {
			vim_free(swapname);
			return FAIL;
		    }
		    eap->forceit = TRUE;
		}
		else
#endif
		{
		    semsg(_(e_swap_file_exists_str_silent_overrides), swapname);
		    vim_free(swapname);
		    return FAIL;
		}
	    }
	    vim_free(swapname);
	}
    }
    return OK;
}