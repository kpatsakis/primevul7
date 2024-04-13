static unsigned long sane_truncate_line(struct emit_callback *ecb, char *line, unsigned long len)
{
	const char *cp;
	unsigned long allot;
	size_t l = len;

	if (ecb->truncate)
		return ecb->truncate(line, len);
	cp = line;
	allot = l;
	while (0 < l) {
		(void) utf8_width(&cp, &l);
		if (!cp)
			break; /* truncated in the middle? */
	}
	return allot - l;
}