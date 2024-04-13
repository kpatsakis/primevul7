postalAddressValidate(
	Syntax *syntax,
	struct berval *in )
{
	struct berval bv = *in;
	ber_len_t c;

	for ( c = 0; c < in->bv_len; c++ ) {
		if ( in->bv_val[c] == '\\' ) {
			c++;
			if ( strncasecmp( &in->bv_val[c], "24", STRLENOF( "24" ) ) != 0
				&& strncasecmp( &in->bv_val[c], "5C", STRLENOF( "5C" ) ) != 0 )
			{
				return LDAP_INVALID_SYNTAX;
			}
			continue;
		}

		if ( in->bv_val[c] == '$' ) {
			bv.bv_len = &in->bv_val[c] - bv.bv_val;
			if ( UTF8StringValidate( NULL, &bv ) != LDAP_SUCCESS ) {
				return LDAP_INVALID_SYNTAX;
			}
			bv.bv_val = &in->bv_val[c] + 1;
		}
	}

	bv.bv_len = &in->bv_val[c] - bv.bv_val;
	return UTF8StringValidate( NULL, &bv );
}