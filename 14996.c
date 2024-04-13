nameUIDValidate(
	Syntax *syntax,
	struct berval *in )
{
	int rc;
	struct berval dn, uid;

	if( BER_BVISEMPTY( in ) ) return LDAP_SUCCESS;

	ber_dupbv( &dn, in );
	if( !dn.bv_val ) return LDAP_OTHER;

	/* if there's a "#", try bitStringValidate()... */
	uid.bv_val = strrchr( dn.bv_val, '#' );
	if ( !BER_BVISNULL( &uid ) ) {
		uid.bv_val++;
		uid.bv_len = dn.bv_len - ( uid.bv_val - dn.bv_val );

		rc = bitStringValidate( NULL, &uid );
		if ( rc == LDAP_SUCCESS ) {
			/* in case of success, trim the UID,
			 * otherwise treat it as part of the DN */
			dn.bv_len -= uid.bv_len + 1;
			uid.bv_val[-1] = '\0';
		}
	}

	rc = dnValidate( NULL, &dn );

	ber_memfree( dn.bv_val );
	return rc;
}