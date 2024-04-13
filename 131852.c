trailing_pad(int *padlen, PrintfTarget *target)
{
	while (*padlen < 0)
	{
		dopr_outch(' ', target);
		++(*padlen);
	}
}