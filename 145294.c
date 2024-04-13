decodeuint(
	char *str,
	u_long *val
	)
{
	if (*str == '0') {
		if (*(str + 1) == 'x' || *(str + 1) == 'X')
			return (hextoint(str + 2, val));
		return (octtoint(str, val));
	}
	return (atouint(str, val));
}