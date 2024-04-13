octetStringSubstringsMatch(
	int *matchp,
	slap_mask_t flags,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *value,
	void *assertedValue )
{
	int match = 0;
	SubstringsAssertion *sub = assertedValue;
	struct berval left = *value;
	int i;
	ber_len_t inlen = 0;

	/* Add up asserted input length */
	if ( !BER_BVISNULL( &sub->sa_initial ) ) {
		inlen += sub->sa_initial.bv_len;
	}
	if ( sub->sa_any ) {
		for ( i = 0; !BER_BVISNULL( &sub->sa_any[i] ); i++ ) {
			inlen += sub->sa_any[i].bv_len;
		}
	}
	if ( !BER_BVISNULL( &sub->sa_final ) ) {
		inlen += sub->sa_final.bv_len;
	}

	if ( !BER_BVISNULL( &sub->sa_initial ) ) {
		if ( inlen > left.bv_len ) {
			match = 1;
			goto done;
		}

		match = memcmp( sub->sa_initial.bv_val, left.bv_val,
			sub->sa_initial.bv_len );

		if ( match != 0 ) {
			goto done;
		}

		left.bv_val += sub->sa_initial.bv_len;
		left.bv_len -= sub->sa_initial.bv_len;
		inlen -= sub->sa_initial.bv_len;
	}

	if ( !BER_BVISNULL( &sub->sa_final ) ) {
		if ( inlen > left.bv_len ) {
			match = 1;
			goto done;
		}

		match = memcmp( sub->sa_final.bv_val,
			&left.bv_val[left.bv_len - sub->sa_final.bv_len],
			sub->sa_final.bv_len );

		if ( match != 0 ) {
			goto done;
		}

		left.bv_len -= sub->sa_final.bv_len;
		inlen -= sub->sa_final.bv_len;
	}

	if ( sub->sa_any ) {
		for ( i = 0; !BER_BVISNULL( &sub->sa_any[i] ); i++ ) {
			ber_len_t idx;
			char *p;

retry:
			if ( inlen > left.bv_len ) {
				/* not enough length */
				match = 1;
				goto done;
			}

			if ( BER_BVISEMPTY( &sub->sa_any[i] ) ) {
				continue;
			}

			p = memchr( left.bv_val, *sub->sa_any[i].bv_val, left.bv_len );

			if( p == NULL ) {
				match = 1;
				goto done;
			}

			idx = p - left.bv_val;

			if ( idx >= left.bv_len ) {
				/* this shouldn't happen */
				return LDAP_OTHER;
			}

			left.bv_val = p;
			left.bv_len -= idx;

			if ( sub->sa_any[i].bv_len > left.bv_len ) {
				/* not enough left */
				match = 1;
				goto done;
			}

			match = memcmp( left.bv_val,
				sub->sa_any[i].bv_val,
				sub->sa_any[i].bv_len );

			if ( match != 0 ) {
				left.bv_val++;
				left.bv_len--;
				goto retry;
			}

			left.bv_val += sub->sa_any[i].bv_len;
			left.bv_len -= sub->sa_any[i].bv_len;
			inlen -= sub->sa_any[i].bv_len;
		}
	}

done:
	*matchp = match;
	return LDAP_SUCCESS;
}