csnNormalize(
	slap_mask_t usage,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *val,
	struct berval *normalized,
	void *ctx )
{
	struct berval	cnt, sid, mod;
	char		*ptr;
	ber_len_t	i;

	assert( val != NULL );
	assert( normalized != NULL );

	assert( SLAP_MR_IS_VALUE_OF_SYNTAX( usage ) != 0 );

	if ( BER_BVISEMPTY( val ) ) {
		return LDAP_INVALID_SYNTAX;
	}

	if ( val->bv_len == STRLENOF( "YYYYmmddHHMMSSZ#SSSSSS#ID#ssssss" ) ) {
		/* Openldap <= 2.3 */

		return csnNormalize23( usage, syntax, mr, val, normalized, ctx );
	}

	if ( val->bv_len == STRLENOF( "YYYYmmddHH:MM:SSZ#0xSSSS#I#ssss" ) ) {
		/* Openldap 2.1 */

		return csnNormalize21( usage, syntax, mr, val, normalized, ctx );
	}

	if ( val->bv_len != STRLENOF( "YYYYmmddHHMMSS.uuuuuuZ#SSSSSS#SID#ssssss" ) ) {
		return LDAP_INVALID_SYNTAX;
	}

	ptr = ber_bvchr( val, '#' );
	if ( ptr == NULL || ptr == &val->bv_val[val->bv_len] ) {
		return LDAP_INVALID_SYNTAX;
	}

	if ( ptr - val->bv_val != STRLENOF( "YYYYmmddHHMMSS.uuuuuuZ" ) ) {
		return LDAP_INVALID_SYNTAX;
	}

	cnt.bv_val = ptr + 1;
	cnt.bv_len = val->bv_len - ( cnt.bv_val - val->bv_val );

	ptr = ber_bvchr( &cnt, '#' );
	if ( ptr == NULL || ptr == &val->bv_val[val->bv_len] ) {
		return LDAP_INVALID_SYNTAX;
	}

	if ( ptr - cnt.bv_val != STRLENOF( "000000" ) ) {
		return LDAP_INVALID_SYNTAX;
	}

	sid.bv_val = ptr + 1;
	sid.bv_len = val->bv_len - ( sid.bv_val - val->bv_val );
		
	ptr = ber_bvchr( &sid, '#' );
	if ( ptr == NULL || ptr == &val->bv_val[val->bv_len] ) {
		return LDAP_INVALID_SYNTAX;
	}

	sid.bv_len = ptr - sid.bv_val;
	if ( sid.bv_len != STRLENOF( "000" ) ) {
		return LDAP_INVALID_SYNTAX;
	}

	mod.bv_val = ptr + 1;
	mod.bv_len = val->bv_len - ( mod.bv_val - val->bv_val );

	if ( mod.bv_len != STRLENOF( "000000" ) ) {
		return LDAP_INVALID_SYNTAX;
	}

	ber_dupbv_x( normalized, val, ctx );

	for ( i = STRLENOF( "YYYYmmddHHMMSS.uuuuuuZ#SSSSSS#" );
		i < normalized->bv_len; i++ )
	{
		/* assume it's already validated that's all hex digits */
		normalized->bv_val[ i ] = TOLOWER( normalized->bv_val[ i ] );
	}

	return LDAP_SUCCESS;
}