seen_endbrace(int refnum)
{
    if (!had_endbrace[refnum])
    {
	char_u *p;

	// Trick: check if "@<=" or "@<!" follows, in which case
	// the \1 can appear before the referenced match.
	for (p = regparse; *p != NUL; ++p)
	    if (p[0] == '@' && p[1] == '<' && (p[2] == '!' || p[2] == '='))
		break;
	if (*p == NUL)
	{
	    emsg(_(e_illegal_back_reference));
	    rc_did_emsg = TRUE;
	    return FALSE;
	}
    }
    return TRUE;
}