asn_to_calendar	(
	ASN1_TIME *asn1time,	/* pointer to ASN1_TIME structure */
	struct calendar *pjd	/* pointer to result */
	)
{
	size_t	len;		/* length of ASN1_TIME string */
	char	v[24];		/* writable copy of ASN1_TIME string */
	unsigned long	temp;	/* result from strtoul */

	/*
	 * Extract time string YYMMDDHHMMSSZ from ASN1 time structure.
	 * Or YYYYMMDDHHMMSSZ.
	 * Note that the YY, MM, DD fields start with one, the HH, MM,
	 * SS fields start with zero and the Z character is ignored.
	 * Also note that two-digit years less than 50 map to years greater than
	 * 100. Dontcha love ASN.1? Better than MIL-188.
	 */
	len = asn1time->length;
	REQUIRE(len < sizeof(v));
	(void)strncpy(v, (char *)(asn1time->data), len);
	REQUIRE(len >= 13);
	temp = strtoul(v+len-3, NULL, 10);
	pjd->second = temp;
	v[len-3] = '\0';

	temp = strtoul(v+len-5, NULL, 10);
	pjd->minute = temp;
	v[len-5] = '\0';

	temp = strtoul(v+len-7, NULL, 10);
	pjd->hour = temp;
	v[len-7] = '\0';

	temp = strtoul(v+len-9, NULL, 10);
	pjd->monthday = temp;
	v[len-9] = '\0';

	temp = strtoul(v+len-11, NULL, 10);
	pjd->month = temp;
	v[len-11] = '\0';

	temp = strtoul(v, NULL, 10);
	/* handle two-digit years */
	if (temp < 50UL)
	    temp += 100UL;
	if (temp < 150UL)
	    temp += 1900UL;
	pjd->year = temp;

	pjd->yearday = pjd->weekday = 0;
	return;
}