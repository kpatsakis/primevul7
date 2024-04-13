int gnutls_x509_ext_import_name_constraints(const gnutls_datum_t * ext,
					 gnutls_x509_name_constraints_t nc,
					 unsigned int flags)
{
	int result, ret;
	ASN1_TYPE c2 = ASN1_TYPE_EMPTY;

	result = asn1_create_element
	    (_gnutls_get_pkix(), "PKIX1.NameConstraints", &c2);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	result = asn1_der_decoding(&c2, ext->data, ext->size, NULL);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		ret = _gnutls_asn2err(result);
		goto cleanup;
	}

	if (!(flags & GNUTLS_NAME_CONSTRAINTS_FLAG_APPEND)
	    || (nc->permitted == NULL && nc->excluded == NULL)) {
		ret =
		    _gnutls_extract_name_constraints(c2, "permittedSubtrees",
						     &nc->permitted);
		if (ret < 0) {
			gnutls_assert();
			goto cleanup;
		}
	}

	ret =
	    _gnutls_extract_name_constraints(c2, "excludedSubtrees",
					     &nc->excluded);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	ret = 0;

 cleanup:
	asn1_delete_structure(&c2);

	return ret;
}