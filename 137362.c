int gnutls_x509_ext_export_key_purposes(gnutls_x509_key_purposes_t p,
				     gnutls_datum_t * ext)
{
	int result, ret;
	ASN1_TYPE c2 = ASN1_TYPE_EMPTY;
	unsigned i;

	result = asn1_create_element
	    (_gnutls_get_pkix(), "PKIX1.ExtKeyUsageSyntax", &c2);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	/* generate the extension.
	 */
	for (i=0;i<p->size;i++) {
		/* 1. create a new element.
		 */
		result = asn1_write_value(c2, "", "NEW", 1);
		if (result != ASN1_SUCCESS) {
			gnutls_assert();
			ret = _gnutls_asn2err(result);
			goto cleanup;
		}

		/* 2. Add the OID.
		 */
		result = asn1_write_value(c2, "?LAST", p->oid[i].data, 1);
		if (result != ASN1_SUCCESS) {
			gnutls_assert();
			ret = _gnutls_asn2err(result);
			goto cleanup;
		}
	}

	ret = _gnutls_x509_der_encode(c2, "", ext, 0);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	ret = 0;

 cleanup:
	asn1_delete_structure(&c2);
	return ret;
}