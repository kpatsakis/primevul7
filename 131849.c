dostr(const char *str, int slen, PrintfTarget *target)
{
	while (slen > 0)
	{
		int			avail;

		if (target->bufend != NULL)
			avail = target->bufend - target->bufptr;
		else
			avail = slen;
		if (avail <= 0)
		{
			/* buffer full, can we dump to stream? */
			if (target->stream == NULL)
				return;			/* no, lose the data */
			flushbuffer(target);
			continue;
		}
		avail = Min(avail, slen);
		memmove(target->bufptr, str, avail);
		target->bufptr += avail;
		str += avail;
		slen -= avail;
	}
}