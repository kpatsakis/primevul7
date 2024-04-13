ExpandMappings(
    regmatch_T	*regmatch,
    int		*num_file,
    char_u	***file)
{
    mapblock_T	*mp;
    int		hash;
    int		count;
    int		round;
    char_u	*p;
    int		i;

    validate_maphash();

    *num_file = 0;		    /* return values in case of FAIL */
    *file = NULL;

    /*
     * round == 1: Count the matches.
     * round == 2: Build the array to keep the matches.
     */
    for (round = 1; round <= 2; ++round)
    {
	count = 0;

	for (i = 0; i < 7; ++i)
	{
	    if (i == 0)
		p = (char_u *)"<silent>";
	    else if (i == 1)
		p = (char_u *)"<unique>";
#ifdef FEAT_EVAL
	    else if (i == 2)
		p = (char_u *)"<script>";
	    else if (i == 3)
		p = (char_u *)"<expr>";
#endif
#ifdef FEAT_LOCALMAP
	    else if (i == 4 && !expand_buffer)
		p = (char_u *)"<buffer>";
#endif
	    else if (i == 5)
		p = (char_u *)"<nowait>";
	    else if (i == 6)
		p = (char_u *)"<special>";
	    else
		continue;

	    if (vim_regexec(regmatch, p, (colnr_T)0))
	    {
		if (round == 1)
		    ++count;
		else
		    (*file)[count++] = vim_strsave(p);
	    }
	}

	for (hash = 0; hash < 256; ++hash)
	{
	    if (expand_isabbrev)
	    {
		if (hash > 0)	/* only one abbrev list */
		    break; /* for (hash) */
		mp = first_abbr;
	    }
#ifdef FEAT_LOCALMAP
	    else if (expand_buffer)
		mp = curbuf->b_maphash[hash];
#endif
	    else
		mp = maphash[hash];
	    for (; mp; mp = mp->m_next)
	    {
		if (mp->m_mode & expand_mapmodes)
		{
		    p = translate_mapping(mp->m_keys);
		    if (p != NULL && vim_regexec(regmatch, p, (colnr_T)0))
		    {
			if (round == 1)
			    ++count;
			else
			{
			    (*file)[count++] = p;
			    p = NULL;
			}
		    }
		    vim_free(p);
		}
	    } /* for (mp) */
	} /* for (hash) */

	if (count == 0)			/* no match found */
	    break; /* for (round) */

	if (round == 1)
	{
	    *file = (char_u **)alloc((unsigned)(count * sizeof(char_u *)));
	    if (*file == NULL)
		return FAIL;
	}
    } /* for (round) */

    if (count > 1)
    {
	char_u	**ptr1;
	char_u	**ptr2;
	char_u	**ptr3;

	/* Sort the matches */
	sort_strings(*file, count);

	/* Remove multiple entries */
	ptr1 = *file;
	ptr2 = ptr1 + 1;
	ptr3 = ptr1 + count;

	while (ptr2 < ptr3)
	{
	    if (STRCMP(*ptr1, *ptr2))
		*++ptr1 = *ptr2++;
	    else
	    {
		vim_free(*ptr2++);
		count--;
	    }
	}
    }

    *num_file = count;
    return (count == 0 ? FAIL : OK);
}