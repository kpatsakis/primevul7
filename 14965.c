numericStringValidate(
	Syntax *syntax,
	struct berval *in )
{
	ber_len_t i;

	if( BER_BVISEMPTY( in ) ) return LDAP_INVALID_SYNTAX;

	for(i=0; i < in->bv_len; i++) {
		if( !SLAP_NUMERIC(in->bv_val[i]) ) {
			return LDAP_INVALID_SYNTAX;
		}
	}

	return LDAP_SUCCESS;
}