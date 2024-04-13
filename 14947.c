attributeCertificateValidate( Syntax *syntax, struct berval *in )
{
	BerElementBuffer berbuf;
	BerElement *ber = (BerElement *)&berbuf;
	ber_tag_t tag;
	ber_len_t len;
	ber_int_t version;
	int cont = 0;

	ber_init2( ber, in, LBER_USE_DER );
	
	tag = ber_skip_tag( ber, &len );	/* Signed wrapper */
	if ( tag != LBER_SEQUENCE ) return LDAP_INVALID_SYNTAX;

	tag = ber_skip_tag( ber, &len );	/* Sequence */
	if ( tag != LBER_SEQUENCE ) return LDAP_INVALID_SYNTAX;

	tag = ber_peek_tag( ber, &len );	/* Version */
	if ( tag != LBER_INTEGER ) return LDAP_INVALID_SYNTAX;
	tag = ber_get_int( ber, &version );	/* X.509 only allows v2 */
	if ( version != SLAP_X509AC_V2 ) return LDAP_INVALID_SYNTAX;

	tag = ber_skip_tag( ber, &len );	/* Holder */
	if ( tag != LBER_SEQUENCE ) return LDAP_INVALID_SYNTAX;
	ber_skip_data( ber, len );

	tag = ber_skip_tag( ber, &len );	/* Issuer */
	if ( tag != SLAP_X509AC_ISSUER ) return LDAP_INVALID_SYNTAX;
	ber_skip_data( ber, len );

	tag = ber_skip_tag( ber, &len );	/* Signature */
	if ( tag != LBER_SEQUENCE ) return LDAP_INVALID_SYNTAX;
	ber_skip_data( ber, len );

	tag = ber_skip_tag( ber, &len );	/* Serial number */
	if ( tag != LBER_INTEGER ) return LDAP_INVALID_SYNTAX;
	ber_skip_data( ber, len );

	tag = ber_skip_tag( ber, &len );	/* AttCertValidityPeriod */
	if ( tag != LBER_SEQUENCE ) return LDAP_INVALID_SYNTAX;
	ber_skip_data( ber, len );

	tag = ber_skip_tag( ber, &len );	/* Attributes */
	if ( tag != LBER_SEQUENCE ) return LDAP_INVALID_SYNTAX;
	ber_skip_data( ber, len );

	tag = ber_peek_tag( ber, &len );

	if ( tag == LBER_BITSTRING ) {	/* issuerUniqueID */
		tag = ber_skip_tag( ber, &len );
		ber_skip_data( ber, len );
		tag = ber_peek_tag( ber, &len );
	}

	if ( tag == LBER_SEQUENCE ) {	/* extensions or signatureAlgorithm */
		tag = ber_skip_tag( ber, &len );
		ber_skip_data( ber, len );
		cont++;
		tag = ber_peek_tag( ber, &len );
	}

	if ( tag == LBER_SEQUENCE ) {	/* signatureAlgorithm */
		tag = ber_skip_tag( ber, &len );
		ber_skip_data( ber, len );
		cont++;
		tag = ber_peek_tag( ber, &len );
	}

	if ( tag == LBER_BITSTRING ) {	/* Signature */
		tag = ber_skip_tag( ber, &len );
		ber_skip_data( ber, len );
		cont++;
		tag = ber_peek_tag( ber, &len );
	}

	/* Must be at end now */
	if ( len != 0 || tag != LBER_DEFAULT || cont < 2 ) return LDAP_INVALID_SYNTAX;

	return LDAP_SUCCESS;
}