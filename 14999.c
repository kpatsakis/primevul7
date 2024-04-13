checkTime( struct berval *in, struct berval *out )
{
	int rc;
	ber_len_t i;
	char buf[STRLENOF("YYYYmmddHHMMSSZ") + 1];
	struct berval bv;

	assert( in != NULL );
	assert( !BER_BVISNULL( in ) );
	assert( !BER_BVISEMPTY( in ) );

	if ( in->bv_len < STRLENOF( "YYmmddHHMMSSZ" ) ) {
		return -1;
	}

	if ( out != NULL ) {
		assert( !BER_BVISNULL( out ) );
		assert( out->bv_len >= sizeof( buf ) );
		bv.bv_val = out->bv_val;

	} else {
		bv.bv_val = buf;
	}

	for ( i = 0; i < STRLENOF( "YYYYmmddHHMMSS" ); i++ ) {
		if ( !ASCII_DIGIT( in->bv_val[i] ) ) break;
	}

	if ( in->bv_val[i] != 'Z' ) {
		return -1;
	}
	i++;

	if ( i != in->bv_len ) {
		return -1;
	}

	if ( i == STRLENOF( "YYYYmmddHHMMSSZ" ) ) {
		lutil_strncopy( bv.bv_val, in->bv_val, i );
		bv.bv_len = i;
		
	} else if ( i == STRLENOF( "YYmmddHHMMSSZ" ) ) {
		char *p = bv.bv_val;
		if ( in->bv_val[0] < '7' ) {
			p = lutil_strcopy( p, "20" );

		} else {
			p = lutil_strcopy( p, "19" );
		}
		lutil_strncopy( p, in->bv_val, i );
		bv.bv_len = 2 + i;

	} else {
		return -1;
	}

	rc = generalizedTimeValidate( NULL, &bv );
	if ( rc == LDAP_SUCCESS && out != NULL ) {
		if ( out->bv_len > bv.bv_len ) {
			out->bv_val[ bv.bv_len ] = '\0';
		}
		out->bv_len = bv.bv_len;
	}

	return rc != LDAP_SUCCESS;
}