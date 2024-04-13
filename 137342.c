int gnutls_x509_ext_export_proxy(int pathLenConstraint, const char *policyLanguage,
			      const char *policy, size_t sizeof_policy,
			      gnutls_datum_t * ext)
{
	ASN1_TYPE c2 = ASN1_TYPE_EMPTY;
	int result;

	result = asn1_create_element(_gnutls_get_pkix(),
				     "PKIX1.ProxyCertInfo", &c2);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	if (pathLenConstraint < 0) {
		result = asn1_write_value(c2, "pCPathLenConstraint", NULL, 0);
		if (result != ASN1_SUCCESS) {
			gnutls_assert();
			result = _gnutls_asn2err(result);
			goto cleanup;
		}
	} else {
		result =
		    _gnutls_x509_write_uint32(c2, "pCPathLenConstraint",
					      pathLenConstraint);

		if (result < 0) {
			gnutls_assert();
			goto cleanup;
		}
	}

	result = asn1_write_value(c2, "proxyPolicy.policyLanguage",
				  policyLanguage, 1);
	if (result < 0) {
		gnutls_assert();
		result = _gnutls_asn2err(result);
		goto cleanup;
	}

	result = asn1_write_value(c2, "proxyPolicy.policy",
				  policy, sizeof_policy);
	if (result < 0) {
		gnutls_assert();
		result = _gnutls_asn2err(result);
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