ctl_putrefid(
	const char *	tag,
	u_int32		refid
	)
{
	char	output[16];
	char *	optr;
	char *	oplim;
	char *	iptr;
	char *	iplim;
	char *	past_eq;

	optr = output;
	oplim = output + sizeof(output);
	while (optr < oplim && '\0' != *tag)
		*optr++ = *tag++;
	if (optr < oplim) {
		*optr++ = '=';
		past_eq = optr;
	}
	if (!(optr < oplim))
		return;
	iptr = (char *)&refid;
	iplim = iptr + sizeof(refid);
	for ( ; optr < oplim && iptr < iplim && '\0' != *iptr;
	     iptr++, optr++)
		if (isprint((int)*iptr))
			*optr = *iptr;
		else
			*optr = '.';
	if (!(optr <= oplim))
		optr = past_eq;
	ctl_putdata(output, (u_int)(optr - output), FALSE);
}