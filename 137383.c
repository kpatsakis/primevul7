int gnutls_x509_ext_export_basic_constraints(unsigned int ca, int pathlen,
					  gnutls_datum_t * ext)
{
	ASN1_TYPE c2 = ASN1_TYPE_EMPTY;
	const char *str;
	int result;

	if (ca == 0)
		str = "FALSE";
	else
		str = "TRUE";

	result =
	    asn1_create_element(_gnutls_get_pkix(),
				"PKIX1.BasicConstraints", &c2);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		result = _gnutls_asn2err(result);
		goto cleanup;
	}

	result = asn1_write_value(c2, "cA", str, 1);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		result = _gnutls_asn2err(result);
		goto cleanup;
	}

	if (pathlen < 0) {
		result = asn1_write_value(c2, "pathLenConstraint", NULL, 0);
		if (result < 0)
			result = _gnutls_asn2err(result);
	} else
		result =
		    _gnutls_x509_write_uint32(c2, "pathLenConstraint", pathlen);
	if (result < 0) {
		gnutls_assert();
		goto cleanup;
	}

	result = _gnutls_x509_der_encode(c2, "", ext, 0);
	if (result < 0) {
		gnutls_assert();
		goto cleanup;
	}

	result = 0;

 cleanup:
	asn1_delete_structure(&c2);
	return result;

}