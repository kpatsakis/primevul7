fmtfloat(double value, char type, int forcesign, int leftjust,
		 int minlen, int zpad, int precision, int pointflag,
		 PrintfTarget *target)
{
	int			signvalue = 0;
	int			prec;
	int			vallen;
	char		fmt[32];
	char		convert[1024];
	int			zeropadlen = 0; /* amount to pad with zeroes */
	int			padlen = 0;		/* amount to pad with spaces */

	/*
	 * We rely on the regular C library's sprintf to do the basic conversion,
	 * then handle padding considerations here.
	 *
	 * The dynamic range of "double" is about 1E+-308 for IEEE math, and not
	 * too wildly more than that with other hardware.  In "f" format, sprintf
	 * could therefore generate at most 308 characters to the left of the
	 * decimal point; while we need to allow the precision to get as high as
	 * 308+17 to ensure that we don't truncate significant digits from very
	 * small values.  To handle both these extremes, we use a buffer of 1024
	 * bytes and limit requested precision to 350 digits; this should prevent
	 * buffer overrun even with non-IEEE math.  If the original precision
	 * request was more than 350, separately pad with zeroes.
	 */
	if (precision < 0)			/* cover possible overflow of "accum" */
		precision = 0;
	prec = Min(precision, 350);

	if (pointflag)
	{
		sprintf(fmt, "%%.%d%c", prec, type);
		zeropadlen = precision - prec;
	}
	else
		sprintf(fmt, "%%%c", type);

	if (!isnan(value) && adjust_sign((value < 0), forcesign, &signvalue))
		value = -value;

	vallen = sprintf(convert, fmt, value);

	/* If it's infinity or NaN, forget about doing any zero-padding */
	if (zeropadlen > 0 && !isdigit((unsigned char) convert[vallen - 1]))
		zeropadlen = 0;

	adjust_padlen(minlen, vallen + zeropadlen, leftjust, &padlen);

	leading_pad(zpad, &signvalue, &padlen, target);

	if (zeropadlen > 0)
	{
		/* If 'e' or 'E' format, inject zeroes before the exponent */
		char	   *epos = strrchr(convert, 'e');

		if (!epos)
			epos = strrchr(convert, 'E');
		if (epos)
		{
			/* pad after exponent */
			dostr(convert, epos - convert, target);
			while (zeropadlen-- > 0)
				dopr_outch('0', target);
			dostr(epos, vallen - (epos - convert), target);
		}
		else
		{
			/* no exponent, pad after the digits */
			dostr(convert, vallen, target);
			while (zeropadlen-- > 0)
				dopr_outch('0', target);
		}
	}
	else
	{
		/* no zero padding, just emit the number as-is */
		dostr(convert, vallen, target);
	}

	trailing_pad(&padlen, target);
}