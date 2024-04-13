dopr(PrintfTarget *target, const char *format, va_list args)
{
	const char *format_start = format;
	int			ch;
	bool		have_dollar;
	bool		have_non_dollar;
	bool		have_star;
	bool		afterstar;
	int			accum;
	int			longlongflag;
	int			longflag;
	int			pointflag;
	int			leftjust;
	int			fieldwidth;
	int			precision;
	int			zpad;
	int			forcesign;
	int			last_dollar;
	int			fmtpos;
	int			cvalue;
	int64		numvalue;
	double		fvalue;
	char	   *strvalue;
	int			i;
	PrintfArgType argtypes[NL_ARGMAX + 1];
	PrintfArgValue argvalues[NL_ARGMAX + 1];

	/*
	 * Parse the format string to determine whether there are %n$ format
	 * specs, and identify the types and order of the format parameters.
	 */
	have_dollar = have_non_dollar = false;
	last_dollar = 0;
	MemSet(argtypes, 0, sizeof(argtypes));

	while ((ch = *format++) != '\0')
	{
		if (ch != '%')
			continue;
		longflag = longlongflag = pointflag = 0;
		fmtpos = accum = 0;
		afterstar = false;
nextch1:
		ch = *format++;
		if (ch == '\0')
			break;				/* illegal, but we don't complain */
		switch (ch)
		{
			case '-':
			case '+':
				goto nextch1;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				accum = accum * 10 + (ch - '0');
				goto nextch1;
			case '.':
				pointflag = 1;
				accum = 0;
				goto nextch1;
			case '*':
				if (afterstar)
					have_non_dollar = true;		/* multiple stars */
				afterstar = true;
				accum = 0;
				goto nextch1;
			case '$':
				have_dollar = true;
				if (accum <= 0 || accum > NL_ARGMAX)
					return -1;
				if (afterstar)
				{
					if (argtypes[accum] &&
						argtypes[accum] != ATYPE_INT)
						return -1;
					argtypes[accum] = ATYPE_INT;
					last_dollar = Max(last_dollar, accum);
					afterstar = false;
				}
				else
					fmtpos = accum;
				accum = 0;
				goto nextch1;
			case 'l':
				if (longflag)
					longlongflag = 1;
				else
					longflag = 1;
				goto nextch1;
			case 'z':
#if SIZEOF_SIZE_T == 8
#ifdef HAVE_LONG_INT_64
				longflag = 1;
#elif defined(HAVE_LONG_LONG_INT_64)
				longlongflag = 1;
#else
#error "Don't know how to print 64bit integers"
#endif
#else
				/* assume size_t is same size as int */
#endif
				goto nextch1;
			case 'h':
			case '\'':
				/* ignore these */
				goto nextch1;
			case 'd':
			case 'i':
			case 'o':
			case 'u':
			case 'x':
			case 'X':
				if (fmtpos)
				{
					PrintfArgType atype;

					if (longlongflag)
						atype = ATYPE_LONGLONG;
					else if (longflag)
						atype = ATYPE_LONG;
					else
						atype = ATYPE_INT;
					if (argtypes[fmtpos] &&
						argtypes[fmtpos] != atype)
						return -1;
					argtypes[fmtpos] = atype;
					last_dollar = Max(last_dollar, fmtpos);
				}
				else
					have_non_dollar = true;
				break;
			case 'c':
				if (fmtpos)
				{
					if (argtypes[fmtpos] &&
						argtypes[fmtpos] != ATYPE_INT)
						return -1;
					argtypes[fmtpos] = ATYPE_INT;
					last_dollar = Max(last_dollar, fmtpos);
				}
				else
					have_non_dollar = true;
				break;
			case 's':
			case 'p':
				if (fmtpos)
				{
					if (argtypes[fmtpos] &&
						argtypes[fmtpos] != ATYPE_CHARPTR)
						return -1;
					argtypes[fmtpos] = ATYPE_CHARPTR;
					last_dollar = Max(last_dollar, fmtpos);
				}
				else
					have_non_dollar = true;
				break;
			case 'e':
			case 'E':
			case 'f':
			case 'g':
			case 'G':
				if (fmtpos)
				{
					if (argtypes[fmtpos] &&
						argtypes[fmtpos] != ATYPE_DOUBLE)
						return -1;
					argtypes[fmtpos] = ATYPE_DOUBLE;
					last_dollar = Max(last_dollar, fmtpos);
				}
				else
					have_non_dollar = true;
				break;
			case '%':
				break;
		}

		/*
		 * If we finish the spec with afterstar still set, there's a
		 * non-dollar star in there.
		 */
		if (afterstar)
			have_non_dollar = true;
	}

	/* Per spec, you use either all dollar or all not. */
	if (have_dollar && have_non_dollar)
		return -1;

	/*
	 * In dollar mode, collect the arguments in physical order.
	 */
	for (i = 1; i <= last_dollar; i++)
	{
		switch (argtypes[i])
		{
			case ATYPE_NONE:
				return -1;		/* invalid format */
			case ATYPE_INT:
				argvalues[i].i = va_arg(args, int);
				break;
			case ATYPE_LONG:
				argvalues[i].l = va_arg(args, long);
				break;
			case ATYPE_LONGLONG:
				argvalues[i].ll = va_arg(args, int64);
				break;
			case ATYPE_DOUBLE:
				argvalues[i].d = va_arg(args, double);
				break;
			case ATYPE_CHARPTR:
				argvalues[i].cptr = va_arg(args, char *);
				break;
		}
	}

	/*
	 * At last we can parse the format for real.
	 */
	format = format_start;
	while ((ch = *format++) != '\0')
	{
		if (ch != '%')
		{
			dopr_outch(ch, target);
			continue;
		}
		fieldwidth = precision = zpad = leftjust = forcesign = 0;
		longflag = longlongflag = pointflag = 0;
		fmtpos = accum = 0;
		have_star = afterstar = false;
nextch2:
		ch = *format++;
		if (ch == '\0')
			break;				/* illegal, but we don't complain */
		switch (ch)
		{
			case '-':
				leftjust = 1;
				goto nextch2;
			case '+':
				forcesign = 1;
				goto nextch2;
			case '0':
				/* set zero padding if no nonzero digits yet */
				if (accum == 0 && !pointflag)
					zpad = '0';
				/* FALL THRU */
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				accum = accum * 10 + (ch - '0');
				goto nextch2;
			case '.':
				if (have_star)
					have_star = false;
				else
					fieldwidth = accum;
				pointflag = 1;
				accum = 0;
				goto nextch2;
			case '*':
				if (have_dollar)
				{
					/* process value after reading n$ */
					afterstar = true;
				}
				else
				{
					/* fetch and process value now */
					int			starval = va_arg(args, int);

					if (pointflag)
					{
						precision = starval;
						if (precision < 0)
						{
							precision = 0;
							pointflag = 0;
						}
					}
					else
					{
						fieldwidth = starval;
						if (fieldwidth < 0)
						{
							leftjust = 1;
							fieldwidth = -fieldwidth;
						}
					}
				}
				have_star = true;
				accum = 0;
				goto nextch2;
			case '$':
				if (afterstar)
				{
					/* fetch and process star value */
					int			starval = argvalues[accum].i;

					if (pointflag)
					{
						precision = starval;
						if (precision < 0)
						{
							precision = 0;
							pointflag = 0;
						}
					}
					else
					{
						fieldwidth = starval;
						if (fieldwidth < 0)
						{
							leftjust = 1;
							fieldwidth = -fieldwidth;
						}
					}
					afterstar = false;
				}
				else
					fmtpos = accum;
				accum = 0;
				goto nextch2;
			case 'l':
				if (longflag)
					longlongflag = 1;
				else
					longflag = 1;
				goto nextch2;
			case 'z':
#if SIZEOF_SIZE_T == 8
#ifdef HAVE_LONG_INT_64
				longflag = 1;
#elif defined(HAVE_LONG_LONG_INT_64)
				longlongflag = 1;
#else
#error "Don't know how to print 64bit integers"
#endif
#else
				/* assume size_t is same size as int */
#endif
				goto nextch2;
			case 'h':
			case '\'':
				/* ignore these */
				goto nextch2;
			case 'd':
			case 'i':
				if (!have_star)
				{
					if (pointflag)
						precision = accum;
					else
						fieldwidth = accum;
				}
				if (have_dollar)
				{
					if (longlongflag)
						numvalue = argvalues[fmtpos].ll;
					else if (longflag)
						numvalue = argvalues[fmtpos].l;
					else
						numvalue = argvalues[fmtpos].i;
				}
				else
				{
					if (longlongflag)
						numvalue = va_arg(args, int64);
					else if (longflag)
						numvalue = va_arg(args, long);
					else
						numvalue = va_arg(args, int);
				}
				fmtint(numvalue, ch, forcesign, leftjust, fieldwidth, zpad,
					   precision, pointflag, target);
				break;
			case 'o':
			case 'u':
			case 'x':
			case 'X':
				if (!have_star)
				{
					if (pointflag)
						precision = accum;
					else
						fieldwidth = accum;
				}
				if (have_dollar)
				{
					if (longlongflag)
						numvalue = (uint64) argvalues[fmtpos].ll;
					else if (longflag)
						numvalue = (unsigned long) argvalues[fmtpos].l;
					else
						numvalue = (unsigned int) argvalues[fmtpos].i;
				}
				else
				{
					if (longlongflag)
						numvalue = (uint64) va_arg(args, int64);
					else if (longflag)
						numvalue = (unsigned long) va_arg(args, long);
					else
						numvalue = (unsigned int) va_arg(args, int);
				}
				fmtint(numvalue, ch, forcesign, leftjust, fieldwidth, zpad,
					   precision, pointflag, target);
				break;
			case 'c':
				if (!have_star)
				{
					if (pointflag)
						precision = accum;
					else
						fieldwidth = accum;
				}
				if (have_dollar)
					cvalue = (unsigned char) argvalues[fmtpos].i;
				else
					cvalue = (unsigned char) va_arg(args, int);
				fmtchar(cvalue, leftjust, fieldwidth, target);
				break;
			case 's':
				if (!have_star)
				{
					if (pointflag)
						precision = accum;
					else
						fieldwidth = accum;
				}
				if (have_dollar)
					strvalue = argvalues[fmtpos].cptr;
				else
					strvalue = va_arg(args, char *);
				fmtstr(strvalue, leftjust, fieldwidth, precision, pointflag,
					   target);
				break;
			case 'p':
				/* fieldwidth/leftjust are ignored ... */
				if (have_dollar)
					strvalue = argvalues[fmtpos].cptr;
				else
					strvalue = va_arg(args, char *);
				fmtptr((void *) strvalue, target);
				break;
			case 'e':
			case 'E':
			case 'f':
			case 'g':
			case 'G':
				if (!have_star)
				{
					if (pointflag)
						precision = accum;
					else
						fieldwidth = accum;
				}
				if (have_dollar)
					fvalue = argvalues[fmtpos].d;
				else
					fvalue = va_arg(args, double);
				fmtfloat(fvalue, ch, forcesign, leftjust,
						 fieldwidth, zpad,
						 precision, pointflag,
						 target);
				break;
			case '%':
				dopr_outch('%', target);
				break;
		}
	}

	return 0;
}