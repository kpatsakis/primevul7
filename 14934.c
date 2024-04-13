integerBitOrMatch(
	int *matchp,
	slap_mask_t flags,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *value,
	void *assertedValue )
{
	SLAP_LONG lValue, lAssertedValue;

	errno = 0;
	/* safe to assume integers are NUL terminated? */
	lValue = SLAP_STRTOL(value->bv_val, NULL, 10);
	if( errno == ERANGE )
	{
		return LDAP_CONSTRAINT_VIOLATION;
	}

	lAssertedValue = SLAP_STRTOL( ((struct berval *)assertedValue)->bv_val,
		NULL, 10);
	if( errno == ERANGE )
	{
		return LDAP_CONSTRAINT_VIOLATION;
	}

	*matchp = ((lValue & lAssertedValue) != 0) ? 0 : -1;
	return LDAP_SUCCESS;
}