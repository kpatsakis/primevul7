nfa_get_match_text(nfa_state_T *start)
{
    nfa_state_T *p = start;
    int		len = 0;
    char_u	*ret;
    char_u	*s;

    if (p->c != NFA_MOPEN)
	return NULL; // just in case
    p = p->out;
    while (p->c > 0)
    {
	len += MB_CHAR2LEN(p->c);
	p = p->out;
    }
    if (p->c != NFA_MCLOSE || p->out->c != NFA_MATCH)
	return NULL;

    ret = alloc(len);
    if (ret != NULL)
    {
	p = start->out->out; // skip first char, it goes into regstart
	s = ret;
	while (p->c > 0)
	{
	    if (has_mbyte)
		s += (*mb_char2bytes)(p->c, s);
	    else
		*s++ = p->c;
	    p = p->out;
	}
	*s = NUL;
    }
    return ret;
}