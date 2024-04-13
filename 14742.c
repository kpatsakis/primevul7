do_source_buffer_init(source_cookie_T *sp, exarg_T *eap)
{
    linenr_T	curr_lnum;
    char_u	*line = NULL;
    char_u	*fname;

    CLEAR_FIELD(*sp);

    if (curbuf == NULL)
	return NULL;

    // Use ":source buffer=<num>" as the script name
    vim_snprintf((char *)IObuff, IOSIZE, ":source buffer=%d", curbuf->b_fnum);
    fname = vim_strsave(IObuff);
    if (fname == NULL)
	return NULL;

    ga_init2(&sp->buflines, sizeof(char_u *), 100);

    // Copy the lines from the buffer into a grow array
    for (curr_lnum = eap->line1; curr_lnum <= eap->line2; curr_lnum++)
    {
	line = vim_strsave(ml_get(curr_lnum));
	if (line == NULL)
	    goto errret;
	if (ga_add_string(&sp->buflines, line) == FAIL)
	    goto errret;
	line = NULL;
    }
    sp->buf_lnum = 0;
    sp->source_from_buf = TRUE;

    return fname;

errret:
    vim_free(fname);
    vim_free(line);
    ga_clear_strings(&sp->buflines);
    return NULL;
}