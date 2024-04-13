integerMatch(
	int *matchp,
	slap_mask_t flags,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *value,
	void *assertedValue )
{
	struct berval *asserted = (struct berval *) assertedValue;
	int vsign = 1, asign = 1;	/* default sign = '+' */
	struct berval v, a;
	int match;

	v = *value;
	if( v.bv_val[0] == '-' ) {
		vsign = -1;
		v.bv_val++;
		v.bv_len--;
	}

	if( BER_BVISEMPTY( &v ) ) vsign = 0;

	a = *asserted;
	if( a.bv_val[0] == '-' ) {
		asign = -1;
		a.bv_val++;
		a.bv_len--;
	}

	if( BER_BVISEMPTY( &a ) ) vsign = 0;

	match = vsign - asign;
	if( match == 0 ) {
		match = ( v.bv_len != a.bv_len
			? ( v.bv_len < a.bv_len ? -1 : 1 )
			: memcmp( v.bv_val, a.bv_val, v.bv_len ));
		if( vsign < 0 ) match = -match;
	}

	/* Ordering rule used in extensible match filter? */
	if ( (flags & SLAP_MR_EXT) && (mr->smr_usage & SLAP_MR_ORDERING) )
		match = (match >= 0);

	*matchp = match;
	return LDAP_SUCCESS;
}