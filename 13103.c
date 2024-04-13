reg(
    int		paren,	// REG_NOPAREN, REG_PAREN, REG_NPAREN or REG_ZPAREN
    int		*flagp)
{
    char_u	*ret;
    char_u	*br;
    char_u	*ender;
    int		parno = 0;
    int		flags;

    *flagp = HASWIDTH;		// Tentatively.

#ifdef FEAT_SYN_HL
    if (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	if (regnzpar >= NSUBEXP)
	    EMSG_RET_NULL(_(e_too_many_z));
	parno = regnzpar;
	regnzpar++;
	ret = regnode(ZOPEN + parno);
    }
    else
#endif
	if (paren == REG_PAREN)
    {
	// Make a MOPEN node.
	if (regnpar >= NSUBEXP)
	    EMSG2_RET_NULL(_(e_too_many_str_open), reg_magic == MAGIC_ALL);
	parno = regnpar;
	++regnpar;
	ret = regnode(MOPEN + parno);
    }
    else if (paren == REG_NPAREN)
    {
	// Make a NOPEN node.
	ret = regnode(NOPEN);
    }
    else
	ret = NULL;

    // Pick up the branches, linking them together.
    br = regbranch(&flags);
    if (br == NULL)
	return NULL;
    if (ret != NULL)
	regtail(ret, br);	// [MZ]OPEN -> first.
    else
	ret = br;
    // If one of the branches can be zero-width, the whole thing can.
    // If one of the branches has * at start or matches a line-break, the
    // whole thing can.
    if (!(flags & HASWIDTH))
	*flagp &= ~HASWIDTH;
    *flagp |= flags & (SPSTART | HASNL | HASLOOKBH);
    while (peekchr() == Magic('|'))
    {
	skipchr();
	br = regbranch(&flags);
	if (br == NULL || reg_toolong)
	    return NULL;
	regtail(ret, br);	// BRANCH -> BRANCH.
	if (!(flags & HASWIDTH))
	    *flagp &= ~HASWIDTH;
	*flagp |= flags & (SPSTART | HASNL | HASLOOKBH);
    }

    // Make a closing node, and hook it on the end.
    ender = regnode(
#ifdef FEAT_SYN_HL
	    paren == REG_ZPAREN ? ZCLOSE + parno :
#endif
	    paren == REG_PAREN ? MCLOSE + parno :
	    paren == REG_NPAREN ? NCLOSE : END);
    regtail(ret, ender);

    // Hook the tails of the branches to the closing node.
    for (br = ret; br != NULL; br = regnext(br))
	regoptail(br, ender);

    // Check for proper termination.
    if (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
#ifdef FEAT_SYN_HL
	if (paren == REG_ZPAREN)
	    EMSG_RET_NULL(_(e_unmatched_z));
	else
#endif
	    if (paren == REG_NPAREN)
	    EMSG2_RET_NULL(_(e_unmatched_str_percent_open), reg_magic == MAGIC_ALL);
	else
	    EMSG2_RET_NULL(_(e_unmatched_str_open), reg_magic == MAGIC_ALL);
    }
    else if (paren == REG_NOPAREN && peekchr() != NUL)
    {
	if (curchr == Magic(')'))
	    EMSG2_RET_NULL(_(e_unmatched_str_close), reg_magic == MAGIC_ALL);
	else
	    EMSG_RET_NULL(_(e_trailing_characters));	// "Can't happen".
	// NOTREACHED
    }
    // Here we set the flag allowing back references to this set of
    // parentheses.
    if (paren == REG_PAREN)
	had_endbrace[parno] = TRUE;	// have seen the close paren
    return ret;
}