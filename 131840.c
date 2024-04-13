fmtptr(void *value, PrintfTarget *target)
{
	int			vallen;
	char		convert[64];

	/* we rely on regular C library's sprintf to do the basic conversion */
	vallen = sprintf(convert, "%p", value);

	dostr(convert, vallen, target);
}