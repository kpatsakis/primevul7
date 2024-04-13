octetStringMatch(
	int *matchp,
	slap_mask_t flags,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *value,
	void *assertedValue )
{
	struct berval *asserted = (struct berval *) assertedValue;
	ber_slen_t d = (ber_slen_t) value->bv_len - (ber_slen_t) asserted->bv_len;

	/* For speed, order first by length, then by contents */
	*matchp = d ? (sizeof(d) == sizeof(int) ? d : d < 0 ? -1 : 1)
		: memcmp( value->bv_val, asserted->bv_val, value->bv_len );

	return LDAP_SUCCESS;
}