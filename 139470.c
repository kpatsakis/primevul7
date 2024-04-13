static int putint(jas_stream_t *out, int sgnd, int prec, long val)
{
	int n;
	int c;
	if (sgnd) {
		/* XXX - Do something here. */
		abort();
	}
	val &= (1 << prec) - 1;
	n = (prec + 7) / 8;
	while (--n >= 0) {
		c = (val >> (n * 8)) & 0xff;
		if (jas_stream_putc(out, c) != c)
			return -1;
	}
	return 0;
}