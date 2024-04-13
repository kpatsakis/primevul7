decodets(
	char *str,
	l_fp *lfp
	)
{
	char *cp;
	char buf[30];
	size_t b;

	/*
	 * If it starts with a 0x, decode as hex.
	 */
	if (*str == '0' && (*(str+1) == 'x' || *(str+1) == 'X'))
		return hextolfp(str+2, lfp);

	/*
	 * If it starts with a '"', try it as an RT-11 date.
	 */
	if (*str == '"') {
		cp = str + 1;
		b = 0;
		while ('"' != *cp && '\0' != *cp &&
		       b < COUNTOF(buf) - 1)
			buf[b++] = *cp++;
		buf[b] = '\0';
		return rtdatetolfp(buf, lfp);
	}

	/*
	 * Might still be hex.  Check out the first character.  Talk
	 * about heuristics!
	 */
	if ((*str >= 'A' && *str <= 'F') || (*str >= 'a' && *str <= 'f'))
		return hextolfp(str, lfp);

	/*
	 * Try it as a decimal.  If this fails, try as an unquoted
	 * RT-11 date.  This code should go away eventually.
	 */
	if (atolfp(str, lfp))
		return 1;

	return rtdatetolfp(str, lfp);
}