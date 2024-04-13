integerFilter(
	slap_mask_t use,
	slap_mask_t flags,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *prefix,
	void * assertedValue,
	BerVarray *keysp,
	void *ctx )
{
	char ibuf[64];
	struct berval iv;
	BerVarray keys;
	struct berval *value;
	int rc;

	value = (struct berval *) assertedValue;

	keys = slap_sl_malloc( sizeof( struct berval ) * 2, ctx );

	keys[0].bv_len = index_intlen;
	keys[0].bv_val = slap_sl_malloc( index_intlen, ctx );
	keys[1].bv_len = 0;
	keys[1].bv_val = NULL;

	iv.bv_len = value->bv_len < index_intlen_strlen + INDEX_INTLEN_CHOP-1
		? value->bv_len : index_intlen_strlen + INDEX_INTLEN_CHOP-1;
	if ( iv.bv_len > (int) sizeof(ibuf) ) {
		iv.bv_val = slap_sl_malloc( iv.bv_len, ctx );
	} else {
		iv.bv_val = ibuf;
		iv.bv_len = sizeof(ibuf);
	}

	rc = integerVal2Key( value, keys, &iv, ctx );

	if ( iv.bv_val != ibuf ) {
		slap_sl_free( iv.bv_val, ctx );
	}

	if ( rc == 0 )
		*keysp = keys;
	else
		slap_sl_free( keys, ctx );

	return rc;
}