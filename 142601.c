gnutls_ocsp_req_set_nonce(gnutls_ocsp_req_t req,
			  unsigned int critical,
			  const gnutls_datum_t * nonce)
{
	int ret;
	gnutls_datum_t dernonce;
	unsigned char temp[SIZEOF_UNSIGNED_LONG_INT + 1];
	int len;

	if (req == NULL || nonce == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	asn1_length_der(nonce->size, temp, &len);

	dernonce.size = 1 + len + nonce->size;
	dernonce.data = gnutls_malloc(dernonce.size);
	if (dernonce.data == NULL) {
		gnutls_assert();
		return GNUTLS_E_MEMORY_ERROR;
	}

	dernonce.data[0] = '\x04';
	memcpy(dernonce.data + 1, temp, len);
	memcpy(dernonce.data + 1 + len, nonce->data, nonce->size);

	ret = _gnutls_set_extension(req->req, "tbsRequest.requestExtensions",
			    GNUTLS_OCSP_NONCE, &dernonce, critical);
	gnutls_free(dernonce.data);
	if (ret != GNUTLS_E_SUCCESS) {
		gnutls_assert();
		return ret;
	}

	return ret;
}