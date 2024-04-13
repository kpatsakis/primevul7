sortassoc(void)
{
	if (numassoc > 1)
		qsort(assoc_cache, (size_t)numassoc,
		      sizeof(assoc_cache[0]), &assoccmp);
}