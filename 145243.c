assoccmp(
	const void *t1,
	const void *t2
	)
{
	const struct association *ass1 = t1;
	const struct association *ass2 = t2;

	if (ass1->assid < ass2->assid)
		return -1;
	if (ass1->assid > ass2->assid)
		return 1;
	return 0;
}