ExpandBufnames(
    char_u	*pat,
    int		*num_file,
    char_u	***file,
    int		options)
{
    int		count = 0;
    buf_T	*buf;
    int		round;
    char_u	*p;
    int		attempt;
    char_u	*patc = NULL;
#ifdef FEAT_VIMINFO
    bufmatch_T	*matches = NULL;
#endif
    int		fuzzy;
    fuzmatch_str_T  *fuzmatch = NULL;

    *num_file = 0;		    // return values in case of FAIL
    *file = NULL;

#ifdef FEAT_DIFF
    if ((options & BUF_DIFF_FILTER) && !curwin->w_p_diff)
	return FAIL;
#endif

    fuzzy = cmdline_fuzzy_complete(pat);

    // Make a copy of "pat" and change "^" to "\(^\|[\/]\)" (if doing regular
    // expression matching)
    if (!fuzzy)
    {
	if (*pat == '^')
	{
	    patc = alloc(STRLEN(pat) + 11);
	    if (patc == NULL)
		return FAIL;
	    STRCPY(patc, "\\(^\\|[\\/]\\)");
	    STRCPY(patc + 11, pat + 1);
	}
	else
	    patc = pat;
    }

    // attempt == 0: try match with    '\<', match at start of word
    // attempt == 1: try match without '\<', match anywhere
    for (attempt = 0; attempt <= (fuzzy ? 0 : 1); ++attempt)
    {
	regmatch_T	regmatch;
	int		score = 0;

	if (!fuzzy)
	{
	    if (attempt > 0 && patc == pat)
		break;	// there was no anchor, no need to try again
	    regmatch.regprog = vim_regcomp(patc + attempt * 11, RE_MAGIC);
	    if (regmatch.regprog == NULL)
	    {
		if (patc != pat)
		    vim_free(patc);
		return FAIL;
	    }
	}

	// round == 1: Count the matches.
	// round == 2: Build the array to keep the matches.
	for (round = 1; round <= 2; ++round)
	{
	    count = 0;
	    FOR_ALL_BUFFERS(buf)
	    {
		if (!buf->b_p_bl)	// skip unlisted buffers
		    continue;
#ifdef FEAT_DIFF
		if (options & BUF_DIFF_FILTER)
		    // Skip buffers not suitable for
		    // :diffget or :diffput completion.
		    if (buf == curbuf || !diff_mode_buf(buf))
			continue;
#endif

		if (!fuzzy)
		    p = buflist_match(&regmatch, buf, p_wic);
		else
		{
		    p = NULL;
		    // first try matching with the short file name
		    if ((score = fuzzy_match_str(buf->b_sfname, pat)) != 0)
			p = buf->b_sfname;
		    if (p == NULL)
		    {
			// next try matching with the full path file name
			if ((score = fuzzy_match_str(buf->b_ffname, pat)) != 0)
			    p = buf->b_ffname;
		    }
		}

		if (p == NULL)
		    continue;

		if (round == 1)
		{
		    ++count;
		    continue;
		}

		if (options & WILD_HOME_REPLACE)
		    p = home_replace_save(buf, p);
		else
		    p = vim_strsave(p);

		if (!fuzzy)
		{
#ifdef FEAT_VIMINFO
		    if (matches != NULL)
		    {
			matches[count].buf = buf;
			matches[count].match = p;
			count++;
		    }
		    else
#endif
			(*file)[count++] = p;
		}
		else
		{
		    fuzmatch[count].idx = count;
		    fuzmatch[count].str = p;
		    fuzmatch[count].score = score;
		    count++;
		}
	    }
	    if (count == 0)	// no match found, break here
		break;
	    if (round == 1)
	    {
		if (!fuzzy)
		{
		    *file = ALLOC_MULT(char_u *, count);
		    if (*file == NULL)
		    {
			vim_regfree(regmatch.regprog);
			if (patc != pat)
			    vim_free(patc);
			return FAIL;
		    }
#ifdef FEAT_VIMINFO
		    if (options & WILD_BUFLASTUSED)
			matches = ALLOC_MULT(bufmatch_T, count);
#endif
		}
		else
		{
		    fuzmatch = ALLOC_MULT(fuzmatch_str_T, count);
		    if (fuzmatch == NULL)
		    {
			*num_file = 0;
			*file = NULL;
			return FAIL;
		    }
		}
	    }
	}

	if (!fuzzy)
	{
	    vim_regfree(regmatch.regprog);
	    if (count)		// match(es) found, break here
		break;
	}
    }

    if (!fuzzy && patc != pat)
	vim_free(patc);

#ifdef FEAT_VIMINFO
    if (!fuzzy)
    {
	if (matches != NULL)
	{
	    int i;
	    if (count > 1)
		qsort(matches, count, sizeof(bufmatch_T), buf_compare);
	    // if the current buffer is first in the list, place it at the end
	    if (matches[0].buf == curbuf)
	    {
		for (i = 1; i < count; i++)
		    (*file)[i-1] = matches[i].match;
		(*file)[count-1] = matches[0].match;
	    }
	    else
	    {
		for (i = 0; i < count; i++)
		    (*file)[i] = matches[i].match;
	    }
	    vim_free(matches);
	}
    }
    else
    {
	if (fuzzymatches_to_strmatches(fuzmatch, file, count, FALSE) == FAIL)
	    return FAIL;
    }
#endif

    *num_file = count;
    return (count == 0 ? FAIL : OK);
}