checkNum( struct berval *in, struct berval *out )
{
	/* parse serialNumber */
	ber_len_t neg = 0, extra = 0;
	char first = '\0';

	out->bv_val = in->bv_val;
	out->bv_len = 0;

	if ( out->bv_val[0] == '-' ) {
		neg++;
		out->bv_len++;
	}

	if ( strncasecmp( out->bv_val, "0x", STRLENOF("0x") ) == 0 ) {
		first = out->bv_val[2];
		extra = 2;

		out->bv_len += STRLENOF("0x");
		for ( ; out->bv_len < in->bv_len; out->bv_len++ ) {
			if ( !ASCII_HEX( out->bv_val[out->bv_len] ) ) break;
		}

	} else if ( out->bv_val[0] == '\'' ) {
		first = out->bv_val[1];
		extra = 3;

		out->bv_len += STRLENOF("'");

		for ( ; out->bv_len < in->bv_len; out->bv_len++ ) {
			if ( !ASCII_HEX( out->bv_val[out->bv_len] ) ) break;
		}
		if ( strncmp( &out->bv_val[out->bv_len], "'H", STRLENOF("'H") ) != 0 ) {
			return -1;
		}
		out->bv_len += STRLENOF("'H");

	} else {
		first = out->bv_val[0];
		for ( ; out->bv_len < in->bv_len; out->bv_len++ ) {
			if ( !ASCII_DIGIT( out->bv_val[out->bv_len] ) ) break;
		}
	}

	if ( !( out->bv_len > neg ) ) {
		return -1;
	}

	if ( ( out->bv_len > extra + 1 + neg ) && ( first == '0' ) ) {
		return -1;
	}

	return 0;
}