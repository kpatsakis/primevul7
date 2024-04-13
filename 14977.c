generalizedTimeNormalize(
	slap_mask_t usage,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *val,
	struct berval *normalized,
	void *ctx )
{
	int parts[9], rc;
	unsigned int len;
	struct berval fraction;

	rc = check_time_syntax(val, 0, parts, &fraction);
	if (rc != LDAP_SUCCESS) {
		return rc;
	}

	len = STRLENOF("YYYYmmddHHMMSSZ") + fraction.bv_len;
	normalized->bv_val = slap_sl_malloc( len + 1, ctx );
	if ( BER_BVISNULL( normalized ) ) {
		return LBER_ERROR_MEMORY;
	}

	sprintf( normalized->bv_val, "%02d%02d%02d%02d%02d%02d%02d",
		parts[0], parts[1], parts[2] + 1, parts[3] + 1,
		parts[4], parts[5], parts[6] );
	if ( !BER_BVISEMPTY( &fraction ) ) {
		memcpy( normalized->bv_val + STRLENOF("YYYYmmddHHMMSSZ")-1,
			fraction.bv_val, fraction.bv_len );
		normalized->bv_val[STRLENOF("YYYYmmddHHMMSSZ")-1] = '.';
	}
	strcpy( normalized->bv_val + len-1, "Z" );
	normalized->bv_len = len;

	return LDAP_SUCCESS;
}