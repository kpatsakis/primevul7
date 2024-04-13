fmtstr(char *value, int leftjust, int minlen, int maxwidth,
	   int pointflag, PrintfTarget *target)
{
	int			padlen,
				vallen;			/* amount to pad */

	/*
	 * If a maxwidth (precision) is specified, we must not fetch more bytes
	 * than that.
	 */
	if (pointflag)
		vallen = pg_strnlen(value, maxwidth);
	else
		vallen = strlen(value);

	adjust_padlen(minlen, vallen, leftjust, &padlen);

	while (padlen > 0)
	{
		dopr_outch(' ', target);
		--padlen;
	}

	dostr(value, vallen, target);

	trailing_pad(&padlen, target);
}