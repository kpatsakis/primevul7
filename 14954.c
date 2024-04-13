UUIDValidate(
	Syntax *syntax,
	struct berval *in )
{
	int i;
	if( in->bv_len != 36 ) {
		return LDAP_INVALID_SYNTAX;
	}

	for( i=0; i<36; i++ ) {
		switch(i) {
			case 8:
			case 13:
			case 18:
			case 23:
				if( in->bv_val[i] != '-' ) {
					return LDAP_INVALID_SYNTAX;
				}
				break;
			default:
				if( !ASCII_HEX( in->bv_val[i]) ) {
					return LDAP_INVALID_SYNTAX;
				}
		}
	}
	
	return LDAP_SUCCESS;
}