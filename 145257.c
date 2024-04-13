helpsort(
	const void *t1,
	const void *t2
	)
{
	const char * const *	name1 = t1;
	const char * const *	name2 = t2;

	return strcmp(*name1, *name2);
}