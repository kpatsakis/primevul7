xmemdup(const char *s, size_t n)
{
	char *p;

	p = xmalloc(n);
	memcpy(p, s, n);

	return p;
}