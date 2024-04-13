gnutls_ocsp_req_add_cert(gnutls_ocsp_req_t req,
			 gnutls_digest_algorithm_t digest,
			 gnutls_x509_crt_t issuer, gnutls_x509_crt_t cert)
{
	int ret;
	gnutls_datum_t sn, tmp, inh, ikh;
	uint8_t inh_buf[MAX_HASH_SIZE];
	uint8_t ikh_buf[MAX_HASH_SIZE];
	size_t inhlen = MAX_HASH_SIZE;
	size_t ikhlen = MAX_HASH_SIZE;

	if (req == NULL || issuer == NULL || cert == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	ret = _gnutls_x509_der_encode(cert->cert,
				      "tbsCertificate.issuer.rdnSequence",
				      &tmp, 0);
	if (ret != GNUTLS_E_SUCCESS) {
		gnutls_assert();
		return ret;
	}

	ret = gnutls_fingerprint(digest, &tmp, inh_buf, &inhlen);
	gnutls_free(tmp.data);
	if (ret != GNUTLS_E_SUCCESS) {
		gnutls_assert();
		return ret;
	}
	inh.size = inhlen;
	inh.data = inh_buf;

	ret = _gnutls_x509_read_value
	    (issuer->cert,
	     "tbsCertificate.subjectPublicKeyInfo.subjectPublicKey", &tmp);
	if (ret != GNUTLS_E_SUCCESS) {
		gnutls_assert();
		return ret;
	}

	ret = gnutls_fingerprint(digest, &tmp, ikh_buf, &ikhlen);
	gnutls_free(tmp.data);
	if (ret != GNUTLS_E_SUCCESS) {
		gnutls_assert();
		return ret;
	}
	ikh.size = ikhlen;
	ikh.data = ikh_buf;

	ret =
	    _gnutls_x509_read_value(cert->cert,
				    "tbsCertificate.serialNumber", &sn);
	if (ret != GNUTLS_E_SUCCESS) {
		gnutls_assert();
		return ret;
	}

	ret = gnutls_ocsp_req_add_cert_id(req, digest, &inh, &ikh, &sn);
	gnutls_free(sn.data);
	if (ret != GNUTLS_E_SUCCESS) {
		gnutls_assert();
		return ret;
	}

	return GNUTLS_E_SUCCESS;
}