telephoneNumberNormalize(
	slap_mask_t usage,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *val,
	struct berval *normalized,
	void *ctx )
{
	char *q;
	ber_len_t c;

	assert( SLAP_MR_IS_VALUE_OF_SYNTAX( usage ) != 0 );

	/* Ensure q is big enough, though validator should have caught this */
	if ( BER_BVISEMPTY( val )) {
		BER_BVZERO( normalized );
		return LDAP_INVALID_SYNTAX;
	}

	q = normalized->bv_val = slap_sl_malloc( val->bv_len + 1, ctx );

	for( c = 0; c < val->bv_len; c++ ) {
		if ( ! ( ASCII_SPACE( val->bv_val[c] ) || val->bv_val[c] == '-' )) {
			*q++ = val->bv_val[c];
		}
	}
	if ( q == normalized->bv_val ) {
		*q++ = ' ';
	}
	*q = '\0';

	normalized->bv_len = q - normalized->bv_val;

	return LDAP_SUCCESS;
}