copy_sub_off(regsub_T *to, regsub_T *from)
{
    if (to->in_use < from->in_use)
	to->in_use = from->in_use;
    if (from->in_use > 1)
    {
	// Copy the match start and end positions.
	if (REG_MULTI)
	    mch_memmove(&to->list.multi[1],
			&from->list.multi[1],
			sizeof(struct multipos) * (from->in_use - 1));
	else
	    mch_memmove(&to->list.line[1],
			&from->list.line[1],
			sizeof(struct linepos) * (from->in_use - 1));
    }
}