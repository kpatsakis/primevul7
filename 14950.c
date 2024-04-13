integerIndexer(
	slap_mask_t use,
	slap_mask_t flags,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *prefix,
	BerVarray values,
	BerVarray *keysp,
	void *ctx )
{
	char ibuf[64];
	struct berval itmp;
	BerVarray keys;
	ber_len_t vlen;
	int i, rc;
	unsigned maxstrlen = index_intlen_strlen + INDEX_INTLEN_CHOP-1;

	/* count the values and find max needed length */
	vlen = 0;
	for( i = 0; !BER_BVISNULL( &values[i] ); i++ ) {
		if ( vlen < values[i].bv_len )
			vlen = values[i].bv_len;
	}
	if ( vlen > maxstrlen )
		vlen = maxstrlen;

	/* we should have at least one value at this point */
	assert( i > 0 );

	keys = slap_sl_malloc( sizeof( struct berval ) * (i+1), ctx );
	for ( i = 0; !BER_BVISNULL( &values[i] ); i++ ) {
		keys[i].bv_len = index_intlen;
		keys[i].bv_val = slap_sl_malloc( index_intlen, ctx );
	}
	keys[i].bv_len = 0;
	keys[i].bv_val = NULL;

	if ( vlen > sizeof(ibuf) ) {
		itmp.bv_val = slap_sl_malloc( vlen, ctx );
	} else {
		itmp.bv_val = ibuf;
	}
	itmp.bv_len = sizeof(ibuf);

	for ( i=0; !BER_BVISNULL( &values[i] ); i++ ) {
		if ( itmp.bv_val != ibuf ) {
			itmp.bv_len = values[i].bv_len;
			if ( itmp.bv_len <= sizeof(ibuf) )
				itmp.bv_len = sizeof(ibuf);
			else if ( itmp.bv_len > maxstrlen )
				itmp.bv_len = maxstrlen;
		}
		rc = integerVal2Key( &values[i], &keys[i], &itmp, ctx );
		if ( rc ) {
			slap_sl_free( keys, ctx );
			goto func_leave;
		}
	}
	*keysp = keys;
func_leave:
	if ( itmp.bv_val != ibuf ) {
		slap_sl_free( itmp.bv_val, ctx );
	}
	return rc;
}