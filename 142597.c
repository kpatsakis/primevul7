gnutls_ocsp_resp_get_extension(gnutls_ocsp_resp_t resp,
			       unsigned indx,
			       gnutls_datum_t * oid,
			       unsigned int *critical,
			       gnutls_datum_t * data)
{
	int ret;
	char str_critical[10];
	char name[ASN1_MAX_NAME_SIZE];
	int len;

	if (!resp) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	snprintf(name, sizeof(name),
		 "tbsResponseData.responseExtensions.?%u.critical",
		 indx + 1);
	len = sizeof(str_critical);
	ret = asn1_read_value(resp->basicresp, name, str_critical, &len);
	if (ret == ASN1_ELEMENT_NOT_FOUND)
		return GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE;
	else if (ret != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(ret);
	}

	if (critical) {
		if (str_critical[0] == 'T')
			*critical = 1;
		else
			*critical = 0;
	}

	if (oid) {
		snprintf(name, sizeof(name),
			 "tbsResponseData.responseExtensions.?%u.extnID",
			 indx + 1);
		ret = _gnutls_x509_read_value(resp->basicresp, name, oid);
		if (ret != GNUTLS_E_SUCCESS) {
			gnutls_assert();
			return ret;
		}
	}

	if (data) {
		snprintf(name, sizeof(name),
			 "tbsResponseData.responseExtensions.?%u.extnValue",
			 indx + 1);
		ret = _gnutls_x509_read_value(resp->basicresp, name, data);
		if (ret != GNUTLS_E_SUCCESS) {
			gnutls_assert();
			if (oid)
				gnutls_free(oid->data);
			return ret;
		}
	}

	return GNUTLS_E_SUCCESS;
}