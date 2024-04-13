varfmt(const char * varname)
{
	u_int n;

	for (n = 0; n < COUNTOF(cookedvars); n++)
		if (!strcmp(varname, cookedvars[n].varname))
			return cookedvars[n].fmt;

	return PADDING;
}