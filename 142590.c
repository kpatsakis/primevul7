gnutls_ocsp_resp_get_responder(gnutls_ocsp_resp_t resp,
			       gnutls_datum_t * dn)
{
	int ret;
	size_t l = 0;

	if (resp == NULL || dn == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	dn->data = NULL;
	dn->size = 0;

	ret = _gnutls_x509_parse_dn
	    (resp->basicresp, "tbsResponseData.responderID.byName",
	     NULL, &l);
	if (ret != GNUTLS_E_SHORT_MEMORY_BUFFER) {
		if (ret == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE)
			return 0; /* for backwards compatibility */
		gnutls_assert();
		return ret;
	}

	dn->data = gnutls_malloc(l);
	if (dn->data == NULL) {
		gnutls_assert();
		return GNUTLS_E_MEMORY_ERROR;
	}

	ret = _gnutls_x509_parse_dn
	    (resp->basicresp, "tbsResponseData.responderID.byName",
	     (char *) dn->data, &l);
	if (ret != GNUTLS_E_SUCCESS) {
		gnutls_assert();
		return ret;
	}

	dn->size = l;

	return GNUTLS_E_SUCCESS;
}