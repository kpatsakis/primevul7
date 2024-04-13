xreallocarray(void *p, size_t n, size_t m)
{
	p = reallocarray(p, n, m);
	if (!p)
		fatal("reallocarray:");

	return p;
}