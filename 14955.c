integerValidate(
	Syntax *syntax,
	struct berval *in )
{
	ber_len_t i;
	struct berval val = *in;

	if ( BER_BVISEMPTY( &val ) ) return LDAP_INVALID_SYNTAX;

	if ( val.bv_val[0] == '-' ) {
		val.bv_len--;
		val.bv_val++;

		if( BER_BVISEMPTY( &val ) ) { /* bare "-" */
			return LDAP_INVALID_SYNTAX;
		}

		if( val.bv_val[0] == '0' ) { /* "-0" */
			return LDAP_INVALID_SYNTAX;
		}

	} else if ( val.bv_val[0] == '0' ) {
		if( val.bv_len > 1 ) { /* "0<more>" */
			return LDAP_INVALID_SYNTAX;
		}

		return LDAP_SUCCESS;
	}

	for( i=0; i < val.bv_len; i++ ) {
		if( !ASCII_DIGIT(val.bv_val[i]) ) {
			return LDAP_INVALID_SYNTAX;
		}
	}

	return LDAP_SUCCESS;
}