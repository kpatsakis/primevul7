gnutls_ocsp_resp_get_signature(gnutls_ocsp_resp_t resp,
			       gnutls_datum_t * sig)
{
	int ret;

	if (resp == NULL || sig == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	ret = _gnutls_x509_read_value(resp->basicresp, "signature", sig);
	if (ret != GNUTLS_E_SUCCESS) {
		gnutls_assert();
		return ret;
	}

	return GNUTLS_E_SUCCESS;
}