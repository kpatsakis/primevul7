copy_ze_off(regsub_T *to, regsub_T *from)
{
    if (rex.nfa_has_zend)
    {
	if (REG_MULTI)
	{
	    if (from->list.multi[0].end_lnum >= 0)
	    {
		to->list.multi[0].end_lnum = from->list.multi[0].end_lnum;
		to->list.multi[0].end_col = from->list.multi[0].end_col;
	    }
	}
	else
	{
	    if (from->list.line[0].end != NULL)
		to->list.line[0].end = from->list.line[0].end;
	}
    }
}