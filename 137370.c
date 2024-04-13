int gnutls_x509_ext_export_aia(gnutls_x509_aia_t aia,
					      gnutls_datum_t * ext)
{
	int ret, result;
	ASN1_TYPE c2 = ASN1_TYPE_EMPTY;
	unsigned int i;

	ret = asn1_create_element(_gnutls_get_pkix(),
				  "PKIX1.AuthorityInfoAccessSyntax", &c2);
	if (ret != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(ret);
	}

	/* 1. create a new element.
	 */
	for (i=0;i<aia->size;i++) {
		result = asn1_write_value(c2, "", "NEW", 1);
		if (result != ASN1_SUCCESS) {
			gnutls_assert();
			ret = _gnutls_asn2err(result);
			goto cleanup;
		}

		/* 2. Add the OID.
		 */
		result = asn1_write_value(c2, "?LAST.accessMethod", aia->aia[i].oid.data, 1);
		if (result != ASN1_SUCCESS) {
			gnutls_assert();
			ret = _gnutls_asn2err(result);
			goto cleanup;
		}

		ret =
		    _gnutls_write_general_name(c2,
						   "?LAST.accessLocation",
						   aia->aia[i].san_type,
						   aia->aia[i].san.data,
						   aia->aia[i].san.size);
		if (ret < 0) {
			gnutls_assert();
			goto cleanup;
		}
	}

	ret = _gnutls_x509_der_encode(c2, "", ext, 0);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

 cleanup:
	asn1_delete_structure(&c2);

	return ret;
}