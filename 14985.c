hexValidate(
	Syntax *syntax,
	struct berval *in )
{
	ber_len_t	i;

	assert( in != NULL );
	assert( !BER_BVISNULL( in ) );

	for ( i = 0; i < in->bv_len; i++ ) {
		if ( !ASCII_HEX( in->bv_val[ i ] ) ) {
			return LDAP_INVALID_SYNTAX;
		}
	}

	return LDAP_SUCCESS;
}