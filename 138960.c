get_map_mode(char_u **cmdp, int forceit)
{
    char_u	*p;
    int		modec;
    int		mode;

    p = *cmdp;
    modec = *p++;
    if (modec == 'i')
	mode = INSERT;				/* :imap */
    else if (modec == 'l')
	mode = LANGMAP;				/* :lmap */
    else if (modec == 'c')
	mode = CMDLINE;				/* :cmap */
    else if (modec == 'n' && *p != 'o')		    /* avoid :noremap */
	mode = NORMAL;				/* :nmap */
    else if (modec == 'v')
	mode = VISUAL + SELECTMODE;		/* :vmap */
    else if (modec == 'x')
	mode = VISUAL;				/* :xmap */
    else if (modec == 's')
	mode = SELECTMODE;			/* :smap */
    else if (modec == 'o')
	mode = OP_PENDING;			/* :omap */
    else if (modec == 't')
	mode = TERMINAL;			/* :tmap */
    else
    {
	--p;
	if (forceit)
	    mode = INSERT + CMDLINE;		/* :map ! */
	else
	    mode = VISUAL + SELECTMODE + NORMAL + OP_PENDING;/* :map */
    }

    *cmdp = p;
    return mode;
}