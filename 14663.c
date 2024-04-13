atoi_err(const char *nptr, int *val)
{
	const char *errstr = NULL;
	long long num;

	if (nptr == NULL || *nptr == '\0')
		return "missing";
	num = strtonum(nptr, 0, INT_MAX, &errstr);
	if (errstr == NULL)
		*val = (int)num;
	return errstr;
}