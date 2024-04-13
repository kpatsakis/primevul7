printvars(
	int length,
	const char *data,
	int status,
	int sttype,
	int quiet,
	FILE *fp
	)
{
	if (rawmode)
	    rawprint(sttype, length, data, status, quiet, fp);
	else
	    cookedprint(sttype, length, data, status, quiet, fp);
}