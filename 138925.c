check_map(
    char_u	*keys,
    int		mode,
    int		exact,		/* require exact match */
    int		ign_mod,	/* ignore preceding modifier */
    int		abbr,		/* do abbreviations */
    mapblock_T	**mp_ptr,	/* return: pointer to mapblock or NULL */
    int		*local_ptr)	/* return: buffer-local mapping or NULL */
{
    int		hash;
    int		len, minlen;
    mapblock_T	*mp;
    char_u	*s;
#ifdef FEAT_LOCALMAP
    int		local;
#endif

    validate_maphash();

    len = (int)STRLEN(keys);
#ifdef FEAT_LOCALMAP
    for (local = 1; local >= 0; --local)
#endif
	/* loop over all hash lists */
	for (hash = 0; hash < 256; ++hash)
	{
	    if (abbr)
	    {
		if (hash > 0)		/* there is only one list. */
		    break;
#ifdef FEAT_LOCALMAP
		if (local)
		    mp = curbuf->b_first_abbr;
		else
#endif
		    mp = first_abbr;
	    }
#ifdef FEAT_LOCALMAP
	    else if (local)
		mp = curbuf->b_maphash[hash];
#endif
	    else
		mp = maphash[hash];
	    for ( ; mp != NULL; mp = mp->m_next)
	    {
		/* skip entries with wrong mode, wrong length and not matching
		 * ones */
		if ((mp->m_mode & mode) && (!exact || mp->m_keylen == len))
		{
		    if (len > mp->m_keylen)
			minlen = mp->m_keylen;
		    else
			minlen = len;
		    s = mp->m_keys;
		    if (ign_mod && s[0] == K_SPECIAL && s[1] == KS_MODIFIER
							       && s[2] != NUL)
		    {
			s += 3;
			if (len > mp->m_keylen - 3)
			    minlen = mp->m_keylen - 3;
		    }
		    if (STRNCMP(s, keys, minlen) == 0)
		    {
			if (mp_ptr != NULL)
			    *mp_ptr = mp;
			if (local_ptr != NULL)
#ifdef FEAT_LOCALMAP
			    *local_ptr = local;
#else
			    *local_ptr = 0;
#endif
			return mp->m_str;
		    }
		}
	    }
	}

    return NULL;
}