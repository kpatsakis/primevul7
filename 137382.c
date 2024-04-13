int gnutls_x509_ext_export_authority_key_id(gnutls_x509_aki_t aki,
					 gnutls_datum_t * ext)
{
	ASN1_TYPE c2 = ASN1_TYPE_EMPTY;
	unsigned i;
	int result, ret;

	result =
	    asn1_create_element(_gnutls_get_pkix(),
				"PKIX1.AuthorityKeyIdentifier", &c2);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	if (aki->id.data != NULL) {
		result =
		    asn1_write_value(c2, "keyIdentifier", aki->id.data,
				     aki->id.size);
		if (result != ASN1_SUCCESS) {
			gnutls_assert();
			ret = _gnutls_asn2err(result);
			goto cleanup;
		}
	} else {
		asn1_write_value(c2, "keyIdentifier", NULL, 0);
	}

	if (aki->serial.data != NULL) {
		result =
		    asn1_write_value(c2, "authorityCertSerialNumber",
				     aki->serial.data, aki->serial.size);
		if (result != ASN1_SUCCESS) {
			gnutls_assert();
			ret = _gnutls_asn2err(result);
			goto cleanup;
		}
	} else {
		asn1_write_value(c2, "authorityCertSerialNumber", NULL, 0);
	}

	if (aki->cert_issuer.size == 0) {
		asn1_write_value(c2, "authorityCertIssuer", NULL, 0);
	} else {
		for (i = 0; i < aki->cert_issuer.size; i++) {
			ret =
			    _gnutls_write_new_general_name(c2,
							   "authorityCertIssuer",
							   aki->cert_issuer.
							   names[i].type,
							   aki->
							   cert_issuer.names[i].
							   san.data,
							   aki->cert_issuer.
							   names[i].san.size);
			if (result < 0) {
				gnutls_assert();
				goto cleanup;
			}
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