fmtint(int64 value, char type, int forcesign, int leftjust,
	   int minlen, int zpad, int precision, int pointflag,
	   PrintfTarget *target)
{
	uint64		base;
	int			dosign;
	const char *cvt = "0123456789abcdef";
	int			signvalue = 0;
	char		convert[64];
	int			vallen = 0;
	int			padlen = 0;		/* amount to pad */
	int			zeropad;		/* extra leading zeroes */

	switch (type)
	{
		case 'd':
		case 'i':
			base = 10;
			dosign = 1;
			break;
		case 'o':
			base = 8;
			dosign = 0;
			break;
		case 'u':
			base = 10;
			dosign = 0;
			break;
		case 'x':
			base = 16;
			dosign = 0;
			break;
		case 'X':
			cvt = "0123456789ABCDEF";
			base = 16;
			dosign = 0;
			break;
		default:
			return;				/* keep compiler quiet */
	}

	/* Handle +/- */
	if (dosign && adjust_sign((value < 0), forcesign, &signvalue))
		value = -value;

	/*
	 * SUS: the result of converting 0 with an explicit precision of 0 is no
	 * characters
	 */
	if (value == 0 && pointflag && precision == 0)
		vallen = 0;
	else
	{
		/* make integer string */
		uint64		uvalue = (uint64) value;

		do
		{
			convert[vallen++] = cvt[uvalue % base];
			uvalue = uvalue / base;
		} while (uvalue);
	}

	zeropad = Max(0, precision - vallen);

	adjust_padlen(minlen, vallen + zeropad, leftjust, &padlen);

	leading_pad(zpad, &signvalue, &padlen, target);

	while (zeropad-- > 0)
		dopr_outch('0', target);

	while (vallen > 0)
		dopr_outch(convert[--vallen], target);

	trailing_pad(&padlen, target);
}