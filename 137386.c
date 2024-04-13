int gnutls_x509_ext_export_private_key_usage_period(time_t activation,
						 time_t expiration,
						 gnutls_datum_t * ext)
{
	int result;
	ASN1_TYPE c2 = ASN1_TYPE_EMPTY;

	result =
	    asn1_create_element(_gnutls_get_pkix(),
				"PKIX1.PrivateKeyUsagePeriod", &c2);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	result = _gnutls_x509_set_time(c2, "notBefore", activation, 1);
	if (result < 0) {
		gnutls_assert();
		goto cleanup;
	}

	result = _gnutls_x509_set_time(c2, "notAfter", expiration, 1);
	if (result < 0) {
		gnutls_assert();
		goto cleanup;
	}

	result = _gnutls_x509_der_encode(c2, "", ext, 0);
	if (result < 0) {
		gnutls_assert();
		goto cleanup;
	}

 cleanup:
	asn1_delete_structure(&c2);

	return result;

}