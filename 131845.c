fmtchar(int value, int leftjust, int minlen, PrintfTarget *target)
{
	int			padlen = 0;		/* amount to pad */

	adjust_padlen(minlen, 1, leftjust, &padlen);

	while (padlen > 0)
	{
		dopr_outch(' ', target);
		--padlen;
	}

	dopr_outch(value, target);

	trailing_pad(&padlen, target);
}