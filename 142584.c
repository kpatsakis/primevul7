gnutls_ocsp_resp_get_response(gnutls_ocsp_resp_t resp,
			      gnutls_datum_t * response_type_oid,
			      gnutls_datum_t * response)
{
	int ret;

	if (resp == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	if (response_type_oid != NULL) {
		ret =
		    _gnutls_x509_read_value(resp->resp,
					    "responseBytes.responseType",
					    response_type_oid);
		if (ret < 0) {
			gnutls_assert();
			return ret;
		}
	}

	if (response != NULL) {
		ret =
		    _gnutls_x509_read_value(resp->resp,
					    "responseBytes.response",
					    response);
		if (ret < 0) {
			gnutls_assert();
			return ret;
		}
	}

	return GNUTLS_E_SUCCESS;
}