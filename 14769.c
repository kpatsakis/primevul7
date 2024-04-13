may_prefix_autoload(char_u *name)
{
    if (SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

	if (si->sn_autoload_prefix != NULL)
	{
	    char_u  *basename = name;
	    size_t  len;
	    char_u  *res;

	    if (*name == K_SPECIAL)
	    {
		char_u *p = vim_strchr(name, '_');

		// skip over "<SNR>99_"
		if (p != NULL)
		    basename = p + 1;
	    }

	    len = STRLEN(si->sn_autoload_prefix) + STRLEN(basename) + 2;
	    res = alloc(len);
	    if (res != NULL)
	    {
		vim_snprintf((char *)res, len, "%s%s",
					     si->sn_autoload_prefix, basename);
		return res;
	    }
	}
    }
    return name;
}