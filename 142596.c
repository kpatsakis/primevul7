gnutls_ocsp_req_get_nonce(gnutls_ocsp_req_t req,
			  unsigned int *critical, gnutls_datum_t * nonce)
{
	int ret;
	gnutls_datum_t tmp;

	if (req == NULL || nonce == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	ret = _gnutls_get_extension(req->req, "tbsRequest.requestExtensions",
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