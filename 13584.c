copy_sub(regsub_T *to, regsub_T *from)
{
    to->in_use = from->in_use;
    if (from->in_use > 0)
    {
	// Copy the match start and end positions.
	if (REG_MULTI)
	    mch_memmove(&to->list.multi[0],
			&from->list.multi[0],
			sizeof(struct multipos) * from->in_use);
	else
	    mch_memmove(&to->list.line[0],
			&from->list.line[0],
			sizeof(struct linepos) * from->in_use);
    }
}