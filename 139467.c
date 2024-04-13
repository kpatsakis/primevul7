static long uptomult(long x, long y)
{
	assert(x >= 0);
	return ((x + y - 1) / y) * y;
}