slap_bin2hex(
	struct berval *in,
	struct berval *out,
	void *ctx )

{	
	/* Use hex format. '123456789abcdef'H */
	unsigned char *ptr, zero = '\0';
	char *sptr;
	int first;
	ber_len_t i, len, nlen;

	assert( in != NULL );
	assert( !BER_BVISNULL( in ) );
	assert( out != NULL );
	assert( !BER_BVISNULL( out ) );

	ptr = (unsigned char *)in->bv_val;
	len = in->bv_len;

	/* Check for minimal encodings */
	if ( len > 1 ) {
		if ( ptr[0] & 0x80 ) {
			if ( ( ptr[0] == 0xff ) && ( ptr[1] & 0x80 ) ) {
				return -1;
			}

		} else if ( ptr[0] == 0 ) {
			if ( !( ptr[1] & 0x80 ) ) {
				return -1;
			}
			len--;
			ptr++;
		}

	} else if ( len == 0 ) {
		/* FIXME: this should not be possible,
		 * since a value of zero would have length 1 */
		len = 1;
		ptr = &zero;
	}

	first = !( ptr[0] & 0xf0U );
	nlen = len * 2 - first + STRLENOF("''H"); /* quotes, H */
	if ( nlen >= out->bv_len ) {
		out->bv_val = slap_sl_malloc( nlen + 1, ctx );
	}
	sptr = out->bv_val;
	*sptr++ = '\'';
	i = 0;
	if ( first ) {
		sprintf( sptr, "%01X", ( ptr[0] & 0x0fU ) );
		sptr++;
		i = 1;
	}
	for ( ; i < len; i++ ) {
		sprintf( sptr, "%02X", ptr[i] );
		sptr += 2;
	}
	*sptr++ = '\'';
	*sptr++ = 'H';
	*sptr = '\0';

	assert( sptr == &out->bv_val[nlen] );

	out->bv_len = nlen;

	return 0;
}