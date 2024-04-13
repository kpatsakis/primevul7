nfa_reg(
    int		paren)	// REG_NOPAREN, REG_PAREN, REG_NPAREN or REG_ZPAREN
{
    int		parno = 0;

    if (paren == REG_PAREN)
    {
	if (regnpar >= NSUBEXP) // Too many `('
	    EMSG_RET_FAIL(_("E872: (NFA regexp) Too many '('"));
	parno = regnpar++;
    }
#ifdef FEAT_SYN_HL
    else if (paren == REG_ZPAREN)
    {
	// Make a ZOPEN node.
	if (regnzpar >= NSUBEXP)
	    EMSG_RET_FAIL(_("E879: (NFA regexp) Too many \\z("));
	parno = regnzpar++;
    }
#endif

    if (nfa_regbranch() == FAIL)
	return FAIL;	    // cascaded error

    while (peekchr() == Magic('|'))
    {
	skipchr();
	if (nfa_regbranch() == FAIL)
	    return FAIL;    // cascaded error
	EMIT(NFA_OR);
    }

    // Check for proper termination.
    if (paren != REG_NOPAREN && getchr() != Magic(')'))
    {
	if (paren == REG_NPAREN)
	    EMSG2_RET_FAIL(_(e_unmatched_str_percent_open),
						       reg_magic == MAGIC_ALL);
	else
	    EMSG2_RET_FAIL(_(e_unmatched_str_open), reg_magic == MAGIC_ALL);
    }
    else if (paren == REG_NOPAREN && peekchr() != NUL)
    {
	if (peekchr() == Magic(')'))
	    EMSG2_RET_FAIL(_(e_unmatched_str_close), reg_magic == MAGIC_ALL);
	else
	    EMSG_RET_FAIL(_("E873: (NFA regexp) proper termination error"));
    }
    /*
     * Here we set the flag allowing back references to this set of
     * parentheses.
     */
    if (paren == REG_PAREN)
    {
	had_endbrace[parno] = TRUE;     // have seen the close paren
	EMIT(NFA_MOPEN + parno);
    }
#ifdef FEAT_SYN_HL
    else if (paren == REG_ZPAREN)
	EMIT(NFA_ZOPEN + parno);
#endif

    return OK;
}