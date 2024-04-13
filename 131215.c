valid_word_prefix(
    int		totprefcnt,	// nr of prefix IDs
    int		arridx,		// idx in sl_pidxs[]
    int		flags,
    char_u	*word,
    slang_T	*slang,
    int		cond_req)	// only use prefixes with a condition
{
    int		prefcnt;
    int		pidx;
    regprog_T	**rp;
    int		prefid;

    prefid = (unsigned)flags >> 24;
    for (prefcnt = totprefcnt - 1; prefcnt >= 0; --prefcnt)
    {
	pidx = slang->sl_pidxs[arridx + prefcnt];

	// Check the prefix ID.
	if (prefid != (pidx & 0xff))
	    continue;

	// Check if the prefix doesn't combine and the word already has a
	// suffix.
	if ((flags & WF_HAS_AFF) && (pidx & WF_PFX_NC))
	    continue;

	// Check the condition, if there is one.  The condition index is
	// stored in the two bytes above the prefix ID byte.
	rp = &slang->sl_prefprog[((unsigned)pidx >> 8) & 0xffff];
	if (*rp != NULL)
	{
	    if (!vim_regexec_prog(rp, FALSE, word, 0))
		continue;
	}
	else if (cond_req)
	    continue;

	// It's a match!  Return the WF_ flags.
	return pidx;
    }
    return 0;
}