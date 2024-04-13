octetStringOrderingMatch(
	int *matchp,
	slap_mask_t flags,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *value,
	void *assertedValue )
{
	struct berval *asserted = (struct berval *) assertedValue;
	ber_len_t v_len  = value->bv_len;
	ber_len_t av_len = asserted->bv_len;

	int match = memcmp( value->bv_val, asserted->bv_val,
		(v_len < av_len ? v_len : av_len) );

	if( match == 0 )
		match = sizeof(v_len) == sizeof(int)
			? (int) v_len - (int) av_len
			: v_len < av_len ? -1 : v_len > av_len;

	/* If used in extensible match filter, match if value < asserted */
	if ( flags & SLAP_MR_EXT )
		match = (match >= 0);

	*matchp = match;
	return LDAP_SUCCESS;
}