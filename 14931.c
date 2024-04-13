uniqueMemberFilter(
	slap_mask_t use,
	slap_mask_t flags,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *prefix,
	void * assertedValue,
	BerVarray *keysp,
	void *ctx )
{
	struct berval *asserted = (struct berval *) assertedValue;
	struct berval assertedDN = *asserted;
	struct berval assertedUID = BER_BVNULL;

	if ( !BER_BVISEMPTY( asserted ) ) {
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

	return octetStringFilter( use, flags, syntax, mr, prefix,
		&assertedDN, keysp, ctx );
}