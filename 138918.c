map_mode_to_chars(int mode)
{
    garray_T    mapmode;

    ga_init2(&mapmode, 1, 7);

    if ((mode & (INSERT + CMDLINE)) == INSERT + CMDLINE)
	ga_append(&mapmode, '!');			/* :map! */
    else if (mode & INSERT)
	ga_append(&mapmode, 'i');			/* :imap */
    else if (mode & LANGMAP)
	ga_append(&mapmode, 'l');			/* :lmap */
    else if (mode & CMDLINE)
	ga_append(&mapmode, 'c');			/* :cmap */
    else if ((mode & (NORMAL + VISUAL + SELECTMODE + OP_PENDING))
				 == NORMAL + VISUAL + SELECTMODE + OP_PENDING)
	ga_append(&mapmode, ' ');			/* :map */
    else
    {
	if (mode & NORMAL)
	    ga_append(&mapmode, 'n');			/* :nmap */
	if (mode & OP_PENDING)
	    ga_append(&mapmode, 'o');			/* :omap */
	if ((mode & (VISUAL + SELECTMODE)) == VISUAL + SELECTMODE)
	    ga_append(&mapmode, 'v');			/* :vmap */
	else
	{
	    if (mode & VISUAL)
		ga_append(&mapmode, 'x');		/* :xmap */
	    if (mode & SELECTMODE)
		ga_append(&mapmode, 's');		/* :smap */
	}
    }

    ga_append(&mapmode, NUL);
    return (char_u *)mapmode.ga_data;
}