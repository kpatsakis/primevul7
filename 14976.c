sidValidate (
	Syntax *syntax,
	struct berval *in )
{
	assert( in != NULL );
	assert( !BER_BVISNULL( in ) );

	if ( in->bv_len != 3 ) {
		return LDAP_INVALID_SYNTAX;
	}

	return hexValidate( NULL, in );
}