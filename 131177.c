match_compoundrule(slang_T *slang, char_u *compflags)
{
    char_u	*p;
    int		i;
    int		c;

    // loop over all the COMPOUNDRULE entries
    for (p = slang->sl_comprules; *p != NUL; ++p)
    {
	// loop over the flags in the compound word we have made, match
	// them against the current rule entry
	for (i = 0; ; ++i)
	{
	    c = compflags[i];
	    if (c == NUL)
		// found a rule that matches for the flags we have so far
		return TRUE;
	    if (*p == '/' || *p == NUL)
		break;  // end of rule, it's too short
	    if (*p == '[')
	    {
		int match = FALSE;

		// compare against all the flags in []
		++p;
		while (*p != ']' && *p != NUL)
		    if (*p++ == c)
			match = TRUE;
		if (!match)
		    break;  // none matches
	    }
	    else if (*p != c)
		break;  // flag of word doesn't match flag in pattern
	    ++p;
	}

	// Skip to the next "/", where the next pattern starts.
	p = vim_strchr(p, '/');
	if (p == NULL)
	    break;
    }

    // Checked all the rules and none of them match the flags, so there
    // can't possibly be a compound starting with these flags.
    return FALSE;
}