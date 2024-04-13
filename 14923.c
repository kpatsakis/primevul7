deliveryMethodValidate(
	Syntax *syntax,
	struct berval *val )
{
#undef LENOF
#define LENOF(s) (sizeof(s)-1)
	struct berval tmp = *val;
	/*
     *	DeliveryMethod = pdm *( WSP DOLLAR WSP DeliveryMethod )
	 *	pdm = "any" / "mhs" / "physical" / "telex" / "teletex" /
	 *		"g3fax" / "g4fax" / "ia5" / "videotex" / "telephone"
	 */
again:
	if( tmp.bv_len < 3 ) return LDAP_INVALID_SYNTAX;

	switch( tmp.bv_val[0] ) {
	case 'a':
	case 'A':
		if(( tmp.bv_len >= LENOF("any") ) &&
			( strncasecmp(tmp.bv_val, "any", LENOF("any")) == 0 ))
		{
			tmp.bv_len -= LENOF("any");
			tmp.bv_val += LENOF("any");
			break;
		}
		return LDAP_INVALID_SYNTAX;

	case 'm':
	case 'M':
		if(( tmp.bv_len >= LENOF("mhs") ) &&
			( strncasecmp(tmp.bv_val, "mhs", LENOF("mhs")) == 0 ))
		{
			tmp.bv_len -= LENOF("mhs");
			tmp.bv_val += LENOF("mhs");
			break;
		}
		return LDAP_INVALID_SYNTAX;

	case 'p':
	case 'P':
		if(( tmp.bv_len >= LENOF("physical") ) &&
			( strncasecmp(tmp.bv_val, "physical", LENOF("physical")) == 0 ))
		{
			tmp.bv_len -= LENOF("physical");
			tmp.bv_val += LENOF("physical");
			break;
		}
		return LDAP_INVALID_SYNTAX;

	case 't':
	case 'T': /* telex or teletex or telephone */
		if(( tmp.bv_len >= LENOF("telex") ) &&
			( strncasecmp(tmp.bv_val, "telex", LENOF("telex")) == 0 ))
		{
			tmp.bv_len -= LENOF("telex");
			tmp.bv_val += LENOF("telex");
			break;
		}
		if(( tmp.bv_len >= LENOF("teletex") ) &&
			( strncasecmp(tmp.bv_val, "teletex", LENOF("teletex")) == 0 ))
		{
			tmp.bv_len -= LENOF("teletex");
			tmp.bv_val += LENOF("teletex");
			break;
		}
		if(( tmp.bv_len >= LENOF("telephone") ) &&
			( strncasecmp(tmp.bv_val, "telephone", LENOF("telephone")) == 0 ))
		{
			tmp.bv_len -= LENOF("telephone");
			tmp.bv_val += LENOF("telephone");
			break;
		}
		return LDAP_INVALID_SYNTAX;

	case 'g':
	case 'G': /* g3fax or g4fax */
		if(( tmp.bv_len >= LENOF("g3fax") ) && (
			( strncasecmp(tmp.bv_val, "g3fax", LENOF("g3fax")) == 0 ) ||
			( strncasecmp(tmp.bv_val, "g4fax", LENOF("g4fax")) == 0 )))
		{
			tmp.bv_len -= LENOF("g3fax");
			tmp.bv_val += LENOF("g3fax");
			break;
		}
		return LDAP_INVALID_SYNTAX;

	case 'i':
	case 'I':
		if(( tmp.bv_len >= LENOF("ia5") ) &&
			( strncasecmp(tmp.bv_val, "ia5", LENOF("ia5")) == 0 ))
		{
			tmp.bv_len -= LENOF("ia5");
			tmp.bv_val += LENOF("ia5");
			break;
		}
		return LDAP_INVALID_SYNTAX;

	case 'v':
	case 'V':
		if(( tmp.bv_len >= LENOF("videotex") ) &&
			( strncasecmp(tmp.bv_val, "videotex", LENOF("videotex")) == 0 ))
		{
			tmp.bv_len -= LENOF("videotex");
			tmp.bv_val += LENOF("videotex");
			break;
		}
		return LDAP_INVALID_SYNTAX;

	default:
		return LDAP_INVALID_SYNTAX;
	}

	if( BER_BVISEMPTY( &tmp ) ) return LDAP_SUCCESS;

	while( !BER_BVISEMPTY( &tmp ) && ( tmp.bv_val[0] == ' ' ) ) {
		tmp.bv_len--;
		tmp.bv_val++;
	}
	if( !BER_BVISEMPTY( &tmp ) && ( tmp.bv_val[0] == '$' ) ) {
		tmp.bv_len--;
		tmp.bv_val++;
	} else {
		return LDAP_INVALID_SYNTAX;
	}
	while( !BER_BVISEMPTY( &tmp ) && ( tmp.bv_val[0] == ' ' ) ) {
		tmp.bv_len--;
		tmp.bv_val++;
	}

	goto again;
}