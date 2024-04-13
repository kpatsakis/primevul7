csnNormalize21(
	slap_mask_t usage,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *val,
	struct berval *normalized,
	void *ctx )
{
	struct berval	gt, cnt, sid, mod;
	struct berval	bv;
	char		buf[ STRLENOF( "YYYYmmddHHMMSS.uuuuuuZ#SSSSSS#SID#ssssss" ) + 1 ];
	char		*ptr;
	ber_len_t	i;

	assert( SLAP_MR_IS_VALUE_OF_SYNTAX( usage ) != 0 );
	assert( !BER_BVISEMPTY( val ) );

	gt = *val;

	ptr = ber_bvchr( &gt, '#' );
	if ( ptr == NULL || ptr == &gt.bv_val[gt.bv_len] ) {
		return LDAP_INVALID_SYNTAX;
	}

	gt.bv_len = ptr - gt.bv_val;
	if ( gt.bv_len != STRLENOF( "YYYYmmddHH:MM:SSZ" ) ) {
		return LDAP_INVALID_SYNTAX;
	}

	if ( gt.bv_val[ 10 ] != ':' || gt.bv_val[ 13 ] != ':' ) {
		return LDAP_INVALID_SYNTAX;
	}

	cnt.bv_val = ptr + 1;
	cnt.bv_len = val->bv_len - ( cnt.bv_val - val->bv_val );

	ptr = ber_bvchr( &cnt, '#' );
	if ( ptr == NULL || ptr == &val->bv_val[val->bv_len] ) {
		return LDAP_INVALID_SYNTAX;
	}

	cnt.bv_len = ptr - cnt.bv_val;
	if ( cnt.bv_len != STRLENOF( "0x0000" ) ) {
		return LDAP_INVALID_SYNTAX;
	}

	if ( strncmp( cnt.bv_val, "0x", STRLENOF( "0x" ) ) != 0 ) {
		return LDAP_INVALID_SYNTAX;
	}

	cnt.bv_val += STRLENOF( "0x" );
	cnt.bv_len -= STRLENOF( "0x" );

	sid.bv_val = ptr + 1;
	sid.bv_len = val->bv_len - ( sid.bv_val - val->bv_val );
		
	ptr = ber_bvchr( &sid, '#' );
	if ( ptr == NULL || ptr == &val->bv_val[val->bv_len] ) {
		return LDAP_INVALID_SYNTAX;
	}

	sid.bv_len = ptr - sid.bv_val;
	if ( sid.bv_len != STRLENOF( "0" ) ) {
		return LDAP_INVALID_SYNTAX;
	}

	mod.bv_val = ptr + 1;
	mod.bv_len = val->bv_len - ( mod.bv_val - val->bv_val );
	if ( mod.bv_len != STRLENOF( "0000" ) ) {
		return LDAP_INVALID_SYNTAX;
	}

	bv.bv_len = STRLENOF( "YYYYmmddHHMMSS.uuuuuuZ#SSSSSS#SID#ssssss" );
	bv.bv_val = buf;

	ptr = bv.bv_val;
	ptr = lutil_strncopy( ptr, gt.bv_val, STRLENOF( "YYYYmmddHH" ) );
	ptr = lutil_strncopy( ptr, &gt.bv_val[ STRLENOF( "YYYYmmddHH:" ) ],
		STRLENOF( "MM" ) );
	ptr = lutil_strncopy( ptr, &gt.bv_val[ STRLENOF( "YYYYmmddHH:MM:" ) ],
		STRLENOF( "SS" ) );
	ptr = lutil_strcopy( ptr, ".000000Z#00" );
	ptr = lutil_strbvcopy( ptr, &cnt );
	*ptr++ = '#';
	*ptr++ = '0';
	*ptr++ = '0';
	*ptr++ = sid.bv_val[ 0 ];
	*ptr++ = '#';
	*ptr++ = '0';
	*ptr++ = '0';
	for ( i = 0; i < mod.bv_len; i++ ) {
		*ptr++ = TOLOWER( mod.bv_val[ i ] );
	}
	*ptr = '\0';

	assert( ptr == &bv.bv_val[bv.bv_len] );

	if ( csnValidate( syntax, &bv ) != LDAP_SUCCESS ) {
		return LDAP_INVALID_SYNTAX;
	}

	ber_dupbv_x( normalized, &bv, ctx );

	return LDAP_SUCCESS;
}