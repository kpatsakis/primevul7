dopr_outch(int c, PrintfTarget *target)
{
	if (target->bufend != NULL && target->bufptr >= target->bufend)
	{
		/* buffer full, can we dump to stream? */
		if (target->stream == NULL)
			return;				/* no, lose the data */
		flushbuffer(target);
	}
	*(target->bufptr++) = c;
}