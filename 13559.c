sub_equal(regsub_T *sub1, regsub_T *sub2)
{
    int		i;
    int		todo;
    linenr_T	s1;
    linenr_T	s2;
    char_u	*sp1;
    char_u	*sp2;

    todo = sub1->in_use > sub2->in_use ? sub1->in_use : sub2->in_use;
    if (REG_MULTI)
    {
	for (i = 0; i < todo; ++i)
	{
	    if (i < sub1->in_use)
		s1 = sub1->list.multi[i].start_lnum;
	    else
		s1 = -1;
	    if (i < sub2->in_use)
		s2 = sub2->list.multi[i].start_lnum;
	    else
		s2 = -1;
	    if (s1 != s2)
		return FALSE;
	    if (s1 != -1 && sub1->list.multi[i].start_col
					     != sub2->list.multi[i].start_col)
		return FALSE;

	    if (rex.nfa_has_backref)
	    {
		if (i < sub1->in_use)
		    s1 = sub1->list.multi[i].end_lnum;
		else
		    s1 = -1;
		if (i < sub2->in_use)
		    s2 = sub2->list.multi[i].end_lnum;
		else
		    s2 = -1;
		if (s1 != s2)
		    return FALSE;
		if (s1 != -1 && sub1->list.multi[i].end_col
					       != sub2->list.multi[i].end_col)
		return FALSE;
	    }
	}
    }
    else
    {
	for (i = 0; i < todo; ++i)
	{
	    if (i < sub1->in_use)
		sp1 = sub1->list.line[i].start;
	    else
		sp1 = NULL;
	    if (i < sub2->in_use)
		sp2 = sub2->list.line[i].start;
	    else
		sp2 = NULL;
	    if (sp1 != sp2)
		return FALSE;
	    if (rex.nfa_has_backref)
	    {
		if (i < sub1->in_use)
		    sp1 = sub1->list.line[i].end;
		else
		    sp1 = NULL;
		if (i < sub2->in_use)
		    sp2 = sub2->list.line[i].end;
		else
		    sp2 = NULL;
		if (sp1 != sp2)
		    return FALSE;
	    }
	}
    }

    return TRUE;
}