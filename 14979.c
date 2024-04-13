uniqueMemberMatch(
	int *matchp,
	slap_mask_t flags,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *value,
	void *assertedValue )
{
	int match;
	struct berval *asserted = (struct berval *) assertedValue;
	struct berval assertedDN = *asserted;
	struct berval assertedUID = BER_BVNULL;
	struct berval valueDN = *value;
	struct berval valueUID = BER_BVNULL;
	int approx = ((flags & SLAP_MR_EQUALITY_APPROX) == SLAP_MR_EQUALITY_APPROX);

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

	if ( !BER_BVISEMPTY( value ) ) {

		valueUID.bv_val = strrchr( valueDN.bv_val, '#' );
		if ( !BER_BVISNULL( &valueUID ) ) {
			valueUID.bv_val++;
			valueUID.bv_len = valueDN.bv_len
				- ( valueUID.bv_val - valueDN.bv_val );

			if ( bitStringValidate( NULL, &valueUID ) == LDAP_SUCCESS ) {
				valueDN.bv_len -= valueUID.bv_len + 1;

			} else {
				BER_BVZERO( &valueUID );
			}
		}
	}

	if( valueUID.bv_len && assertedUID.bv_len ) {
		ber_slen_t d;
		d = (ber_slen_t) valueUID.bv_len - (ber_slen_t) assertedUID.bv_len;
		if ( d ) {
			*matchp = sizeof(d) == sizeof(int) ? d : d < 0 ? -1 : 1;
			return LDAP_SUCCESS;
		}

		match = memcmp( valueUID.bv_val, assertedUID.bv_val, valueUID.bv_len );
		if( match ) {
			*matchp = match;
			return LDAP_SUCCESS;
		}

	} else if ( !approx && valueUID.bv_len ) {
		match = -1;
		*matchp = match;
		return LDAP_SUCCESS;

	} else if ( !approx && assertedUID.bv_len ) {
		match = 1;
		*matchp = match;
		return LDAP_SUCCESS;
	}

	return dnMatch( matchp, flags, syntax, mr, &valueDN, &assertedDN );
}