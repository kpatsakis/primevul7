UTF8StringNormalize(
	slap_mask_t use,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *val,
	struct berval *normalized,
	void *ctx )
{
	struct berval tmp, nvalue;
	int flags, wasspace;
	ber_len_t i;

	assert( SLAP_MR_IS_VALUE_OF_SYNTAX( use ) != 0 );

	if( BER_BVISNULL( val ) ) {
		/* assume we're dealing with a syntax (e.g., UTF8String)
		 * which allows empty strings
		 */
		BER_BVZERO( normalized );
		return LDAP_SUCCESS;
	}

	flags = SLAP_MR_ASSOCIATED( mr, slap_schema.si_mr_caseExactMatch )
		? LDAP_UTF8_NOCASEFOLD : LDAP_UTF8_CASEFOLD;
	flags |= ( ( use & SLAP_MR_EQUALITY_APPROX ) == SLAP_MR_EQUALITY_APPROX )
		? LDAP_UTF8_APPROX : 0;

	val = UTF8bvnormalize( val, &tmp, flags, ctx );
	/* out of memory or syntax error, the former is unlikely */
	if( val == NULL ) {
		return LDAP_INVALID_SYNTAX;
	}
	
	/* collapse spaces (in place) */
	nvalue.bv_len = 0;
	nvalue.bv_val = tmp.bv_val;

	/* trim leading spaces? */
	wasspace = !((( use & SLAP_MR_SUBSTR_ANY ) == SLAP_MR_SUBSTR_ANY ) ||
		(( use & SLAP_MR_SUBSTR_FINAL ) == SLAP_MR_SUBSTR_FINAL ));

	for( i = 0; i < tmp.bv_len; i++) {
		if ( ASCII_SPACE( tmp.bv_val[i] )) {
			if( wasspace++ == 0 ) {
				/* trim repeated spaces */
				nvalue.bv_val[nvalue.bv_len++] = tmp.bv_val[i];
			}
		} else {
			wasspace = 0;
			nvalue.bv_val[nvalue.bv_len++] = tmp.bv_val[i];
		}
	}

	if( !BER_BVISEMPTY( &nvalue ) ) {
		/* trim trailing space? */
		if( wasspace && (
			(( use & SLAP_MR_SUBSTR_INITIAL ) != SLAP_MR_SUBSTR_INITIAL ) &&
			( use & SLAP_MR_SUBSTR_ANY ) != SLAP_MR_SUBSTR_ANY ))
		{
			--nvalue.bv_len;
		}
		nvalue.bv_val[nvalue.bv_len] = '\0';

	} else if ( tmp.bv_len )  {
		/* string of all spaces is treated as one space */
		nvalue.bv_val[0] = ' ';
		nvalue.bv_val[1] = '\0';
		nvalue.bv_len = 1;
	}	/* should never be entered with 0-length val */

	*normalized = nvalue;
	return LDAP_SUCCESS;
}