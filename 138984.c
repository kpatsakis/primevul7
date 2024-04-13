map_to_exists_mode(char_u *rhs, int mode, int abbr)
{
    mapblock_T	*mp;
    int		hash;
# ifdef FEAT_LOCALMAP
    int		exp_buffer = FALSE;

    validate_maphash();

    /* Do it twice: once for global maps and once for local maps. */
    for (;;)
    {
# endif
	for (hash = 0; hash < 256; ++hash)
	{
	    if (abbr)
	    {
		if (hash > 0)		/* there is only one abbr list */
		    break;
#ifdef FEAT_LOCALMAP
		if (exp_buffer)
		    mp = curbuf->b_first_abbr;
		else
#endif
		    mp = first_abbr;
	    }
# ifdef FEAT_LOCALMAP
	    else if (exp_buffer)
		mp = curbuf->b_maphash[hash];
# endif
	    else
		mp = maphash[hash];
	    for (; mp; mp = mp->m_next)
	    {
		if ((mp->m_mode & mode)
			&& strstr((char *)mp->m_str, (char *)rhs) != NULL)
		    return TRUE;
	    }
	}
# ifdef FEAT_LOCALMAP
	if (exp_buffer)
	    break;
	exp_buffer = TRUE;
    }
# endif

    return FALSE;
}