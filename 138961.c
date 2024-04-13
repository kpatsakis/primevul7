do_map(
    int		maptype,
    char_u	*arg,
    int		mode,
    int		abbrev)		/* not a mapping but an abbreviation */
{
    char_u	*keys;
    mapblock_T	*mp, **mpp;
    char_u	*rhs;
    char_u	*p;
    int		n;
    int		len = 0;	/* init for GCC */
    char_u	*newstr;
    int		hasarg;
    int		haskey;
    int		did_it = FALSE;
#ifdef FEAT_LOCALMAP
    int		did_local = FALSE;
#endif
    int		round;
    char_u	*keys_buf = NULL;
    char_u	*arg_buf = NULL;
    int		retval = 0;
    int		do_backslash;
    int		hash;
    int		new_hash;
    mapblock_T	**abbr_table;
    mapblock_T	**map_table;
    int		unique = FALSE;
    int		nowait = FALSE;
    int		silent = FALSE;
    int		special = FALSE;
#ifdef FEAT_EVAL
    int		expr = FALSE;
#endif
    int		noremap;
    char_u      *orig_rhs;

    keys = arg;
    map_table = maphash;
    abbr_table = &first_abbr;

    /* For ":noremap" don't remap, otherwise do remap. */
    if (maptype == 2)
	noremap = REMAP_NONE;
    else
	noremap = REMAP_YES;

    /* Accept <buffer>, <nowait>, <silent>, <expr> <script> and <unique> in
     * any order. */
    for (;;)
    {
#ifdef FEAT_LOCALMAP
	/*
	 * Check for "<buffer>": mapping local to buffer.
	 */
	if (STRNCMP(keys, "<buffer>", 8) == 0)
	{
	    keys = skipwhite(keys + 8);
	    map_table = curbuf->b_maphash;
	    abbr_table = &curbuf->b_first_abbr;
	    continue;
	}
#endif

	/*
	 * Check for "<nowait>": don't wait for more characters.
	 */
	if (STRNCMP(keys, "<nowait>", 8) == 0)
	{
	    keys = skipwhite(keys + 8);
	    nowait = TRUE;
	    continue;
	}

	/*
	 * Check for "<silent>": don't echo commands.
	 */
	if (STRNCMP(keys, "<silent>", 8) == 0)
	{
	    keys = skipwhite(keys + 8);
	    silent = TRUE;
	    continue;
	}

	/*
	 * Check for "<special>": accept special keys in <>
	 */
	if (STRNCMP(keys, "<special>", 9) == 0)
	{
	    keys = skipwhite(keys + 9);
	    special = TRUE;
	    continue;
	}

#ifdef FEAT_EVAL
	/*
	 * Check for "<script>": remap script-local mappings only
	 */
	if (STRNCMP(keys, "<script>", 8) == 0)
	{
	    keys = skipwhite(keys + 8);
	    noremap = REMAP_SCRIPT;
	    continue;
	}

	/*
	 * Check for "<expr>": {rhs} is an expression.
	 */
	if (STRNCMP(keys, "<expr>", 6) == 0)
	{
	    keys = skipwhite(keys + 6);
	    expr = TRUE;
	    continue;
	}
#endif
	/*
	 * Check for "<unique>": don't overwrite an existing mapping.
	 */
	if (STRNCMP(keys, "<unique>", 8) == 0)
	{
	    keys = skipwhite(keys + 8);
	    unique = TRUE;
	    continue;
	}
	break;
    }

    validate_maphash();

    /*
     * Find end of keys and skip CTRL-Vs (and backslashes) in it.
     * Accept backslash like CTRL-V when 'cpoptions' does not contain 'B'.
     * with :unmap white space is included in the keys, no argument possible.
     */
    p = keys;
    do_backslash = (vim_strchr(p_cpo, CPO_BSLASH) == NULL);
    while (*p && (maptype == 1 || !VIM_ISWHITE(*p)))
    {
	if ((p[0] == Ctrl_V || (do_backslash && p[0] == '\\')) &&
								  p[1] != NUL)
	    ++p;		/* skip CTRL-V or backslash */
	++p;
    }
    if (*p != NUL)
	*p++ = NUL;

    p = skipwhite(p);
    rhs = p;
    hasarg = (*rhs != NUL);
    haskey = (*keys != NUL);

    /* check for :unmap without argument */
    if (maptype == 1 && !haskey)
    {
	retval = 1;
	goto theend;
    }

    /*
     * If mapping has been given as ^V<C_UP> say, then replace the term codes
     * with the appropriate two bytes. If it is a shifted special key, unshift
     * it too, giving another two bytes.
     * replace_termcodes() may move the result to allocated memory, which
     * needs to be freed later (*keys_buf and *arg_buf).
     * replace_termcodes() also removes CTRL-Vs and sometimes backslashes.
     */
    if (haskey)
	keys = replace_termcodes(keys, &keys_buf, TRUE, TRUE, special);
    orig_rhs = rhs;
    if (hasarg)
    {
	if (STRICMP(rhs, "<nop>") == 0)	    /* "<Nop>" means nothing */
	    rhs = (char_u *)"";
	else
	    rhs = replace_termcodes(rhs, &arg_buf, FALSE, TRUE, special);
    }

    /*
     * check arguments and translate function keys
     */
    if (haskey)
    {
	len = (int)STRLEN(keys);
	if (len > MAXMAPLEN)		/* maximum length of MAXMAPLEN chars */
	{
	    retval = 1;
	    goto theend;
	}

	if (abbrev && maptype != 1)
	{
	    /*
	     * If an abbreviation ends in a keyword character, the
	     * rest must be all keyword-char or all non-keyword-char.
	     * Otherwise we won't be able to find the start of it in a
	     * vi-compatible way.
	     */
	    if (has_mbyte)
	    {
		int	first, last;
		int	same = -1;

		first = vim_iswordp(keys);
		last = first;
		p = keys + (*mb_ptr2len)(keys);
		n = 1;
		while (p < keys + len)
		{
		    ++n;			/* nr of (multi-byte) chars */
		    last = vim_iswordp(p);	/* type of last char */
		    if (same == -1 && last != first)
			same = n - 1;		/* count of same char type */
		    p += (*mb_ptr2len)(p);
		}
		if (last && n > 2 && same >= 0 && same < n - 1)
		{
		    retval = 1;
		    goto theend;
		}
	    }
	    else if (vim_iswordc(keys[len - 1]))  // ends in keyword char
		    for (n = 0; n < len - 2; ++n)
			if (vim_iswordc(keys[n]) != vim_iswordc(keys[len - 2]))
			{
			    retval = 1;
			    goto theend;
			}
	    /* An abbreviation cannot contain white space. */
	    for (n = 0; n < len; ++n)
		if (VIM_ISWHITE(keys[n]))
		{
		    retval = 1;
		    goto theend;
		}
	}
    }

    if (haskey && hasarg && abbrev)	/* if we will add an abbreviation */
	no_abbr = FALSE;		/* reset flag that indicates there are
							    no abbreviations */

    if (!haskey || (maptype != 1 && !hasarg))
	msg_start();

#ifdef FEAT_LOCALMAP
    /*
     * Check if a new local mapping wasn't already defined globally.
     */
    if (map_table == curbuf->b_maphash && haskey && hasarg && maptype != 1)
    {
	/* need to loop over all global hash lists */
	for (hash = 0; hash < 256 && !got_int; ++hash)
	{
	    if (abbrev)
	    {
		if (hash != 0)	/* there is only one abbreviation list */
		    break;
		mp = first_abbr;
	    }
	    else
		mp = maphash[hash];
	    for ( ; mp != NULL && !got_int; mp = mp->m_next)
	    {
		/* check entries with the same mode */
		if ((mp->m_mode & mode) != 0
			&& mp->m_keylen == len
			&& unique
			&& STRNCMP(mp->m_keys, keys, (size_t)len) == 0)
		{
		    if (abbrev)
			semsg(_("E224: global abbreviation already exists for %s"),
				mp->m_keys);
		    else
			semsg(_("E225: global mapping already exists for %s"),
				mp->m_keys);
		    retval = 5;
		    goto theend;
		}
	    }
	}
    }

    /*
     * When listing global mappings, also list buffer-local ones here.
     */
    if (map_table != curbuf->b_maphash && !hasarg && maptype != 1)
    {
	/* need to loop over all global hash lists */
	for (hash = 0; hash < 256 && !got_int; ++hash)
	{
	    if (abbrev)
	    {
		if (hash != 0)	/* there is only one abbreviation list */
		    break;
		mp = curbuf->b_first_abbr;
	    }
	    else
		mp = curbuf->b_maphash[hash];
	    for ( ; mp != NULL && !got_int; mp = mp->m_next)
	    {
		/* check entries with the same mode */
		if ((mp->m_mode & mode) != 0)
		{
		    if (!haskey)		    /* show all entries */
		    {
			showmap(mp, TRUE);
			did_local = TRUE;
		    }
		    else
		    {
			n = mp->m_keylen;
			if (STRNCMP(mp->m_keys, keys,
					    (size_t)(n < len ? n : len)) == 0)
			{
			    showmap(mp, TRUE);
			    did_local = TRUE;
			}
		    }
		}
	    }
	}
    }
#endif

    /*
     * Find an entry in the maphash[] list that matches.
     * For :unmap we may loop two times: once to try to unmap an entry with a
     * matching 'from' part, a second time, if the first fails, to unmap an
     * entry with a matching 'to' part. This was done to allow ":ab foo bar"
     * to be unmapped by typing ":unab foo", where "foo" will be replaced by
     * "bar" because of the abbreviation.
     */
    for (round = 0; (round == 0 || maptype == 1) && round <= 1
					      && !did_it && !got_int; ++round)
    {
	/* need to loop over all hash lists */
	for (hash = 0; hash < 256 && !got_int; ++hash)
	{
	    if (abbrev)
	    {
		if (hash > 0)	/* there is only one abbreviation list */
		    break;
		mpp = abbr_table;
	    }
	    else
		mpp = &(map_table[hash]);
	    for (mp = *mpp; mp != NULL && !got_int; mp = *mpp)
	    {

		if (!(mp->m_mode & mode))   /* skip entries with wrong mode */
		{
		    mpp = &(mp->m_next);
		    continue;
		}
		if (!haskey)		    /* show all entries */
		{
		    showmap(mp, map_table != maphash);
		    did_it = TRUE;
		}
		else			    /* do we have a match? */
		{
		    if (round)	    /* second round: Try unmap "rhs" string */
		    {
			n = (int)STRLEN(mp->m_str);
			p = mp->m_str;
		    }
		    else
		    {
			n = mp->m_keylen;
			p = mp->m_keys;
		    }
		    if (STRNCMP(p, keys, (size_t)(n < len ? n : len)) == 0)
		    {
			if (maptype == 1)	/* delete entry */
			{
			    /* Only accept a full match.  For abbreviations we
			     * ignore trailing space when matching with the
			     * "lhs", since an abbreviation can't have
			     * trailing space. */
			    if (n != len && (!abbrev || round || n > len
					       || *skipwhite(keys + n) != NUL))
			    {
				mpp = &(mp->m_next);
				continue;
			    }
			    /*
			     * We reset the indicated mode bits. If nothing is
			     * left the entry is deleted below.
			     */
			    mp->m_mode &= ~mode;
			    did_it = TRUE;	/* remember we did something */
			}
			else if (!hasarg)	/* show matching entry */
			{
			    showmap(mp, map_table != maphash);
			    did_it = TRUE;
			}
			else if (n != len)	/* new entry is ambiguous */
			{
			    mpp = &(mp->m_next);
			    continue;
			}
			else if (unique)
			{
			    if (abbrev)
				semsg(_("E226: abbreviation already exists for %s"),
									   p);
			    else
				semsg(_("E227: mapping already exists for %s"), p);
			    retval = 5;
			    goto theend;
			}
			else			/* new rhs for existing entry */
			{
			    mp->m_mode &= ~mode;	/* remove mode bits */
			    if (mp->m_mode == 0 && !did_it) /* reuse entry */
			    {
				newstr = vim_strsave(rhs);
				if (newstr == NULL)
				{
				    retval = 4;		/* no mem */
				    goto theend;
				}
				vim_free(mp->m_str);
				mp->m_str = newstr;
				vim_free(mp->m_orig_str);
				mp->m_orig_str = vim_strsave(orig_rhs);
				mp->m_noremap = noremap;
				mp->m_nowait = nowait;
				mp->m_silent = silent;
				mp->m_mode = mode;
#ifdef FEAT_EVAL
				mp->m_expr = expr;
				mp->m_script_ctx = current_sctx;
				mp->m_script_ctx.sc_lnum += sourcing_lnum;
#endif
				did_it = TRUE;
			    }
			}
			if (mp->m_mode == 0)	/* entry can be deleted */
			{
			    map_free(mpp);
			    continue;		/* continue with *mpp */
			}

			/*
			 * May need to put this entry into another hash list.
			 */
			new_hash = MAP_HASH(mp->m_mode, mp->m_keys[0]);
			if (!abbrev && new_hash != hash)
			{
			    *mpp = mp->m_next;
			    mp->m_next = map_table[new_hash];
			    map_table[new_hash] = mp;

			    continue;		/* continue with *mpp */
			}
		    }
		}
		mpp = &(mp->m_next);
	    }
	}
    }

    if (maptype == 1)			    /* delete entry */
    {
	if (!did_it)
	    retval = 2;			    /* no match */
	else if (*keys == Ctrl_C)
	{
	    /* If CTRL-C has been unmapped, reuse it for Interrupting. */
#ifdef FEAT_LOCALMAP
	    if (map_table == curbuf->b_maphash)
		curbuf->b_mapped_ctrl_c &= ~mode;
	    else
#endif
		mapped_ctrl_c &= ~mode;
	}
	goto theend;
    }

    if (!haskey || !hasarg)		    /* print entries */
    {
	if (!did_it
#ifdef FEAT_LOCALMAP
		&& !did_local
#endif
		)
	{
	    if (abbrev)
		msg(_("No abbreviation found"));
	    else
		msg(_("No mapping found"));
	}
	goto theend;			    /* listing finished */
    }

    if (did_it)			/* have added the new entry already */
	goto theend;

    /*
     * Get here when adding a new entry to the maphash[] list or abbrlist.
     */
    mp = (mapblock_T *)alloc((unsigned)sizeof(mapblock_T));
    if (mp == NULL)
    {
	retval = 4;	    /* no mem */
	goto theend;
    }

    /* If CTRL-C has been mapped, don't always use it for Interrupting. */
    if (*keys == Ctrl_C)
    {
#ifdef FEAT_LOCALMAP
	if (map_table == curbuf->b_maphash)
	    curbuf->b_mapped_ctrl_c |= mode;
	else
#endif
	    mapped_ctrl_c |= mode;
    }

    mp->m_keys = vim_strsave(keys);
    mp->m_str = vim_strsave(rhs);
    mp->m_orig_str = vim_strsave(orig_rhs);
    if (mp->m_keys == NULL || mp->m_str == NULL)
    {
	vim_free(mp->m_keys);
	vim_free(mp->m_str);
	vim_free(mp->m_orig_str);
	vim_free(mp);
	retval = 4;	/* no mem */
	goto theend;
    }
    mp->m_keylen = (int)STRLEN(mp->m_keys);
    mp->m_noremap = noremap;
    mp->m_nowait = nowait;
    mp->m_silent = silent;
    mp->m_mode = mode;
#ifdef FEAT_EVAL
    mp->m_expr = expr;
    mp->m_script_ctx = current_sctx;
    mp->m_script_ctx.sc_lnum += sourcing_lnum;
#endif

    /* add the new entry in front of the abbrlist or maphash[] list */
    if (abbrev)
    {
	mp->m_next = *abbr_table;
	*abbr_table = mp;
    }
    else
    {
	n = MAP_HASH(mp->m_mode, mp->m_keys[0]);
	mp->m_next = map_table[n];
	map_table[n] = mp;
    }

theend:
    vim_free(keys_buf);
    vim_free(arg_buf);
    return retval;
}