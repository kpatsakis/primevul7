hexNormalize(
	slap_mask_t usage,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *val,
	struct berval *normalized,
	void *ctx )
{
	ber_len_t	i;

	assert( val != NULL );
	assert( normalized != NULL );

	ber_dupbv_x( normalized, val, ctx );

	for ( i = 0; i < normalized->bv_len; i++ ) {
		if ( !ASCII_HEX( normalized->bv_val[ i ] ) ) {
			ber_memfree_x( normalized->bv_val, ctx );
			BER_BVZERO( normalized );
			return LDAP_INVALID_SYNTAX;
		}

		normalized->bv_val[ i ] = TOLOWER( normalized->bv_val[ i ] );
	}

	return LDAP_SUCCESS;
}