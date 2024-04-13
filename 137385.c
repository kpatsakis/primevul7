static int encode_user_notice(const gnutls_datum_t * txt,
			      gnutls_datum_t * der_data)
{
	int result;
	ASN1_TYPE c2 = ASN1_TYPE_EMPTY;

	if ((result =
	     asn1_create_element(_gnutls_get_pkix(),
				 "PKIX1.UserNotice", &c2)) != ASN1_SUCCESS) {
		gnutls_assert();
		result = _gnutls_asn2err(result);
		goto error;
	}

	/* delete noticeRef */
	result = asn1_write_value(c2, "noticeRef", NULL, 0);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		result = _gnutls_asn2err(result);
		goto error;
	}

	result = asn1_write_value(c2, "explicitText", "utf8String", 1);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		result = _gnutls_asn2err(result);
		goto error;
	}

	result =
	    asn1_write_value(c2, "explicitText.utf8String", txt->data,
			     txt->size);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		result = _gnutls_asn2err(result);
		goto error;
	}

	result = _gnutls_x509_der_encode(c2, "", der_data, 0);
	if (result < 0) {
		gnutls_assert();
		goto error;
	}

	result = 0;

 error:
	asn1_delete_structure(&c2);
	return result;

}