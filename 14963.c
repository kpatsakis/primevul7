integerVal2Key(
	struct berval *in,
	struct berval *key,
	struct berval *tmp,
	void *ctx )
{
	/* Integer index key format, designed for memcmp to collate correctly:
	 * if too large: one's complement sign*<approx exponent=chopped bytes>,
	 * two's complement value (sign-extended or chopped as needed),
	 * however in first byte above, the top <number of exponent-bytes + 1>
	 * bits are the inverse sign and next bit is the sign as delimiter.
	 */
	ber_slen_t k = index_intlen_strlen;
	ber_len_t chop = 0;
	unsigned signmask = ~0x7fU;
	unsigned char lenbuf[sizeof(k) + 2], *lenp, neg = 0xff;
	struct berval val = *in, itmp = *tmp;

	if ( val.bv_val[0] != '-' ) {
		neg = 0;
		--k;
	}

	/* Chop least significant digits, increase length instead */
	if ( val.bv_len > (ber_len_t) k ) {
		chop = (val.bv_len-k+2)/INDEX_INTLEN_CHOP; /* 2 fewer digits */
		val.bv_len -= chop * INDEX_INTLEN_CHOP;	/* #digits chopped */
		chop *= INDEX_INTLEN_CHOPBYTES;		/* #bytes added */
	}

	if ( lutil_str2bin( &val, &itmp, ctx )) {
		return LDAP_INVALID_SYNTAX;
	}

	/* Omit leading sign byte */
	if ( itmp.bv_val[0] == neg ) {
		itmp.bv_val++;
		itmp.bv_len--;
	}

	k = (ber_slen_t) index_intlen - (ber_slen_t) (itmp.bv_len + chop);
	if ( k > 0 ) {
		assert( chop == 0 );
		memset( key->bv_val, neg, k );	/* sign-extend */
	} else if ( k != 0 || ((itmp.bv_val[0] ^ neg) & 0xc0) ) {
		/* Got exponent -k, or no room for 2 sign bits */
		lenp = lenbuf + sizeof(lenbuf);
		chop = - (ber_len_t) k;
		do {
			*--lenp = ((unsigned char) chop & 0xff) ^ neg;
			signmask >>= 1;
		} while ( (chop >>= 8) != 0 || (signmask >> 1) & (*lenp ^ neg) );
		/* With n bytes in lenbuf, the top n+1 bits of (signmask&0xff)
		 * are 1, and the top n+2 bits of lenp[0] are the sign bit. */
		k = (lenbuf + sizeof(lenbuf)) - lenp;
		if ( k > (ber_slen_t) index_intlen )
			k = index_intlen;
		memcpy( key->bv_val, lenp, k );
		itmp.bv_len = index_intlen - k;
	}
	memcpy( key->bv_val + k, itmp.bv_val, itmp.bv_len );
	key->bv_val[0] ^= (unsigned char) signmask & 0xff; /* invert sign */
	return 0;
}