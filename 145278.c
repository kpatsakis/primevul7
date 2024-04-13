decodeint(
	char *str,
	long *val
	)
{
	if (*str == '0') {
		if (*(str+1) == 'x' || *(str+1) == 'X')
		    return hextoint(str+2, (u_long *)val);
		return octtoint(str, (u_long *)val);
	}
	return atoint(str, val);
}