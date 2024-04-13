uniqueMemberNormalize(
	slap_mask_t usage,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *val,
	struct berval *normalized,
	void *ctx )
{
	struct berval out;
	int rc;

	assert( SLAP_MR_IS_VALUE_OF_SYNTAX( usage ) != 0 );

	ber_dupbv_x( &out, val, ctx );
	if ( BER_BVISEMPTY( &out ) ) {
		*normalized = out;

	} else {
		struct berval uid = BER_BVNULL;

		uid.bv_val = strrchr( out.bv_val, '#' );
		if ( !BER_BVISNULL( &uid ) ) {
			uid.bv_val++;
			uid.bv_len = out.bv_len - ( uid.bv_val - out.bv_val );

			rc = bitStringValidate( NULL, &uid );
			if ( rc == LDAP_SUCCESS ) {
				uid.bv_val[-1] = '\0';
				out.bv_len -= uid.bv_len + 1;
			} else {
				BER_BVZERO( &uid );
			}
		}

		rc = dnNormalize( 0, NULL, NULL, &out, normalized, ctx );

		if( rc != LDAP_SUCCESS ) {
			slap_sl_free( out.bv_val, ctx );
			return LDAP_INVALID_SYNTAX;
		}

		if( !BER_BVISNULL( &uid ) ) {
			char	*tmp;

			tmp = ch_realloc( normalized->bv_val,
				normalized->bv_len + uid.bv_len
				+ STRLENOF("#") + 1 );
			if ( tmp == NULL ) {
				ber_memfree_x( normalized->bv_val, ctx );
				return LDAP_OTHER;
			}

			normalized->bv_val = tmp;

			/* insert the separator */
			normalized->bv_val[normalized->bv_len++] = '#';

			/* append the UID */
			AC_MEMCPY( &normalized->bv_val[normalized->bv_len],
				uid.bv_val, uid.bv_len );
			normalized->bv_len += uid.bv_len;

			/* terminate */
			normalized->bv_val[normalized->bv_len] = '\0';
		}

		slap_sl_free( out.bv_val, ctx );
	}

	return LDAP_SUCCESS;
}