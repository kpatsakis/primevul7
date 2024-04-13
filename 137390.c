int gnutls_x509_aki_get_cert_issuer(gnutls_x509_aki_t aki, unsigned int seq,
				    unsigned int *san_type,
				    gnutls_datum_t *san,
				    gnutls_datum_t *othername_oid,
				    gnutls_datum_t *serial)
{
	if (seq >= aki->cert_issuer.size)
		return gnutls_assert_val(GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE);

	if (aki->serial.size == 0)
		return gnutls_assert_val(GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE);

	if (serial)
		memcpy(serial, &aki->serial, sizeof(gnutls_datum_t));

	if (san) {
		memcpy(san, &aki->cert_issuer.names[seq].san,
		       sizeof(gnutls_datum_t));
	}

	if (othername_oid != NULL
	    && aki->cert_issuer.names[seq].type == GNUTLS_SAN_OTHERNAME) {
		othername_oid->data =
		    aki->cert_issuer.names[seq].othername_oid.data;
		othername_oid->size =
		    aki->cert_issuer.names[seq].othername_oid.size;
	}

	if (san_type)
		*san_type = aki->cert_issuer.names[seq].type;

	return 0;

}