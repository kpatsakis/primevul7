captype(
    char_u	*word,
    char_u	*end)	    // When NULL use up to NUL byte.
{
    char_u	*p;
    int		c;
    int		firstcap;
    int		allcap;
    int		past_second = FALSE;	// past second word char

    // find first letter
    for (p = word; !spell_iswordp_nmw(p, curwin); MB_PTR_ADV(p))
	if (end == NULL ? *p == NUL : p >= end)
	    return 0;	    // only non-word characters, illegal word
    if (has_mbyte)
	c = mb_ptr2char_adv(&p);
    else
	c = *p++;
    firstcap = allcap = SPELL_ISUPPER(c);

    /*
     * Need to check all letters to find a word with mixed upper/lower.
     * But a word with an upper char only at start is a ONECAP.
     */
    for ( ; end == NULL ? *p != NUL : p < end; MB_PTR_ADV(p))
	if (spell_iswordp_nmw(p, curwin))
	{
	    c = PTR2CHAR(p);
	    if (!SPELL_ISUPPER(c))
	    {
		// UUl -> KEEPCAP
		if (past_second && allcap)
		    return WF_KEEPCAP;
		allcap = FALSE;
	    }
	    else if (!allcap)
		// UlU -> KEEPCAP
		return WF_KEEPCAP;
	    past_second = TRUE;
	}

    if (allcap)
	return WF_ALLCAP;
    if (firstcap)
	return WF_ONECAP;
    return 0;
}