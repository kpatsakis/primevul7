printableStringValidate(
	Syntax *syntax,
	struct berval *val )
{
	ber_len_t i;

	if( BER_BVISEMPTY( val ) ) return LDAP_INVALID_SYNTAX;

	for(i=0; i < val->bv_len; i++) {
		if( !SLAP_PRINTABLE(val->bv_val[i]) ) {
			return LDAP_INVALID_SYNTAX;
		}
	}

	return LDAP_SUCCESS;
}