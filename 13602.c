pim_info(nfa_pim_T *pim)
{
    static char buf[30];

    if (pim == NULL || pim->result == NFA_PIM_UNUSED)
	buf[0] = NUL;
    else
    {
	sprintf(buf, " PIM col %d", REG_MULTI ? (int)pim->end.pos.col
		: (int)(pim->end.ptr - rex.input));
    }
    return buf;
}