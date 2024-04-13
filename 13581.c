match_backref(
    regsub_T	*sub,	    // pointers to subexpressions
    int		subidx,
    int		*bytelen)   // out: length of match in bytes
{
    int		len;

    if (sub->in_use <= subidx)
    {
retempty:
	// backref was not set, match an empty string
	*bytelen = 0;
	return TRUE;
    }

    if (REG_MULTI)
    {
	if (sub->list.multi[subidx].start_lnum < 0
				       || sub->list.multi[subidx].end_lnum < 0)
	    goto retempty;
	if (sub->list.multi[subidx].start_lnum == rex.lnum
			       && sub->list.multi[subidx].end_lnum == rex.lnum)
	{
	    len = sub->list.multi[subidx].end_col
					  - sub->list.multi[subidx].start_col;
	    if (cstrncmp(rex.line + sub->list.multi[subidx].start_col,
							 rex.input, &len) == 0)
	    {
		*bytelen = len;
		return TRUE;
	    }
	}
	else
	{
	    if (match_with_backref(
			sub->list.multi[subidx].start_lnum,
			sub->list.multi[subidx].start_col,
			sub->list.multi[subidx].end_lnum,
			sub->list.multi[subidx].end_col,
			bytelen) == RA_MATCH)
		return TRUE;
	}
    }
    else
    {
	if (sub->list.line[subidx].start == NULL
					|| sub->list.line[subidx].end == NULL)
	    goto retempty;
	len = (int)(sub->list.line[subidx].end - sub->list.line[subidx].start);
	if (cstrncmp(sub->list.line[subidx].start, rex.input, &len) == 0)
	{
	    *bytelen = len;
	    return TRUE;
	}
    }
    return FALSE;
}