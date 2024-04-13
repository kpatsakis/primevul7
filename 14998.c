uniqueMemberIndexer(
	slap_mask_t use,
	slap_mask_t flags,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *prefix,
	BerVarray values,
	BerVarray *keysp,
	void *ctx )
{
	BerVarray dnvalues;
	int rc;
	int i;
	for( i=0; !BER_BVISNULL( &values[i] ); i++ ) {
		/* just count them */                 
	}
	assert( i > 0 );

	dnvalues = slap_sl_malloc( sizeof( struct berval ) * (i+1), ctx );

	for( i=0; !BER_BVISNULL( &values[i] ); i++ ) {
		struct berval assertedDN = values[i];
		struct berval assertedUID = BER_BVNULL;

		if ( !BER_BVISEMPTY( &assertedDN ) ) {
			assertedUID.bv_val = strrchr( assertedDN.bv_val, '#' );
			if ( !BER_BVISNULL( &assertedUID ) ) {
				assertedUID.bv_val++;
				assertedUID.bv_len = assertedDN.bv_len
					- ( assertedUID.bv_val - assertedDN.bv_val );
	
				if ( bitStringValidate( NULL, &assertedUID ) == LDAP_SUCCESS ) {
					assertedDN.bv_len -= assertedUID.bv_len + 1;

				} else {
					BER_BVZERO( &assertedUID );
				}
			}
		}

		dnvalues[i] = assertedDN;
	}
	BER_BVZERO( &dnvalues[i] );

	rc = octetStringIndexer( use, flags, syntax, mr, prefix,
		dnvalues, keysp, ctx );

	slap_sl_free( dnvalues, ctx );
	return rc;
}