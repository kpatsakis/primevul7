sequenceValidate(
	Syntax *syntax,
	struct berval *in )
{
	if ( in->bv_len < 2 ) return LDAP_INVALID_SYNTAX;
	if ( in->bv_val[0] != LBER_SEQUENCE ) return LDAP_INVALID_SYNTAX;

	return LDAP_SUCCESS;
}