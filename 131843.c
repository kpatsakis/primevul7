flushbuffer(PrintfTarget *target)
{
	size_t		nc = target->bufptr - target->bufstart;

	if (nc > 0)
		target->nchars += fwrite(target->bufstart, 1, nc, target->stream);
	target->bufptr = target->bufstart;
}