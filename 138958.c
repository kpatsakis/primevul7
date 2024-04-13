showmap(
    mapblock_T	*mp,
    int		local)	    /* TRUE for buffer-local map */
{
    int		len = 1;
    char_u	*mapchars;

    if (message_filtered(mp->m_keys) && message_filtered(mp->m_str))
	return;

    if (msg_didout || msg_silent != 0)
    {
	msg_putchar('\n');
	if (got_int)	    /* 'q' typed at MORE prompt */
	    return;
    }

    mapchars = map_mode_to_chars(mp->m_mode);
    if (mapchars != NULL)
    {
	msg_puts((char *)mapchars);
	len = (int)STRLEN(mapchars);
	vim_free(mapchars);
    }

    while (++len <= 3)
	msg_putchar(' ');

    /* Display the LHS.  Get length of what we write. */
    len = msg_outtrans_special(mp->m_keys, TRUE, 0);
    do
    {
	msg_putchar(' ');		/* padd with blanks */
	++len;
    } while (len < 12);

    if (mp->m_noremap == REMAP_NONE)
	msg_puts_attr("*", HL_ATTR(HLF_8));
    else if (mp->m_noremap == REMAP_SCRIPT)
	msg_puts_attr("&", HL_ATTR(HLF_8));
    else
	msg_putchar(' ');

    if (local)
	msg_putchar('@');
    else
	msg_putchar(' ');

    /* Use FALSE below if we only want things like <Up> to show up as such on
     * the rhs, and not M-x etc, TRUE gets both -- webb */
    if (*mp->m_str == NUL)
	msg_puts_attr("<Nop>", HL_ATTR(HLF_8));
    else
    {
	/* Remove escaping of CSI, because "m_str" is in a format to be used
	 * as typeahead. */
	char_u *s = vim_strsave(mp->m_str);
	if (s != NULL)
	{
	    vim_unescape_csi(s);
	    msg_outtrans_special(s, FALSE, 0);
	    vim_free(s);
	}
    }
#ifdef FEAT_EVAL
    if (p_verbose > 0)
	last_set_msg(mp->m_script_ctx);
#endif
    out_flush();			/* show one line at a time */
}