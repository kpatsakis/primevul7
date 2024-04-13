csnSidNormalize(
	slap_mask_t usage,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *val,
	struct berval *normalized,
	void *ctx )
{
	struct berval	bv;
	char		*ptr,
			buf[ 4 ];


	if ( BER_BVISEMPTY( val ) ) {
		return LDAP_INVALID_SYNTAX;
	}

	if ( SLAP_MR_IS_VALUE_OF_ASSERTION_SYNTAX(usage) ) {
		return sidNormalize( 0, NULL, NULL, val, normalized, ctx );
	}

	assert( SLAP_MR_IS_VALUE_OF_ATTRIBUTE_SYNTAX(usage) != 0 );

	ptr = ber_bvchr( val, '#' );
	if ( ptr == NULL || ptr == &val->bv_val[val->bv_len] ) {
		return LDAP_INVALID_SYNTAX;
	}

	bv.bv_val = ptr + 1;
	bv.bv_len = val->bv_len - ( ptr + 1 - val->bv_val );

	ptr = ber_bvchr( &bv, '#' );
	if ( ptr == NULL || ptr == &val->bv_val[val->bv_len] ) {
		return LDAP_INVALID_SYNTAX;
	}

	bv.bv_val = ptr + 1;
	bv.bv_len = val->bv_len - ( ptr + 1 - val->bv_val );
		
	ptr = ber_bvchr( &bv, '#' );
	if ( ptr == NULL || ptr == &val->bv_val[val->bv_len] ) {
		return LDAP_INVALID_SYNTAX;
	}

	bv.bv_len = ptr - bv.bv_val;

	if ( bv.bv_len == 2 ) {
		/* OpenLDAP 2.3 SID */
		buf[ 0 ] = '0';
		buf[ 1 ] = bv.bv_val[ 0 ];
		buf[ 2 ] = bv.bv_val[ 1 ];
		buf[ 3 ] = '\0';

		bv.bv_val = buf;
		bv.bv_len = 3;
	}

	return sidNormalize( 0, NULL, NULL, &bv, normalized, ctx );
}