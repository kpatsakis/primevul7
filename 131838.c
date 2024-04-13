leading_pad(int zpad, int *signvalue, int *padlen, PrintfTarget *target)
{
	if (*padlen > 0 && zpad)
	{
		if (*signvalue)
		{
			dopr_outch(*signvalue, target);
			--(*padlen);
			*signvalue = 0;
		}
		while (*padlen > 0)
		{
			dopr_outch(zpad, target);
			--(*padlen);
		}
	}
	while (*padlen > (*signvalue != 0))
	{
		dopr_outch(' ', target);
		--(*padlen);
	}
	if (*signvalue)
	{
		dopr_outch(*signvalue, target);
		if (*padlen > 0)
			--(*padlen);
		else if (*padlen < 0)
			++(*padlen);
	}
}