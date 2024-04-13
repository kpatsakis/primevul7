gnutls_ocsp_resp_get_nonce(gnutls_ocsp_resp_t resp,
			   unsigned int *critical, gnutls_datum_t * nonce)
{
	int ret;
	gnutls_datum_t tmp;

	ret =
	    _gnutls_get_extension(resp->basicresp,
			  "tbsResponseData.responseExtensions",
			  GNUTLS_OCSP_NONCE, 0, &tmp, critical);
	if (ret != GNUTLS_E_SUCCESS) {
		gnutls_assert();
		return ret;
	}

	ret =
	    _gnutls_x509_decode_string(ASN1_ETYPE_OCTET_STRING, tmp.data,
				       (size_t) tmp.size, nonce, 0);
	if (ret < 0) {
		gnutls_assert();
		gnutls_free(tmp.data);
		return ret;
	}

	gnutls_free(tmp.data);

	return GNUTLS_E_SUCCESS;
}