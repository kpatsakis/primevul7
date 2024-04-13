xmalloc(size_t n)
{
	void *p;

	p = malloc(n);
	if (!p)
		fatal("malloc:");

	return p;
}