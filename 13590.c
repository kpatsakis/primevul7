log_subexpr(regsub_T *sub)
{
    int j;

    for (j = 0; j < sub->in_use; j++)
	if (REG_MULTI)
	    fprintf(log_fd, "*** group %d, start: c=%d, l=%d, end: c=%d, l=%d\n",
		    j,
		    sub->list.multi[j].start_col,
		    (int)sub->list.multi[j].start_lnum,
		    sub->list.multi[j].end_col,
		    (int)sub->list.multi[j].end_lnum);
	else
	{
	    char *s = (char *)sub->list.line[j].start;
	    char *e = (char *)sub->list.line[j].end;

	    fprintf(log_fd, "*** group %d, start: \"%s\", end: \"%s\"\n",
		    j,
		    s == NULL ? "NULL" : s,
		    e == NULL ? "NULL" : e);
	}
}