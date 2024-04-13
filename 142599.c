_ocsp_resp_verify_direct(gnutls_ocsp_resp_t resp,
			 gnutls_x509_crt_t signercert,
			 unsigned int *verify, unsigned int flags)
{
	gnutls_datum_t sig = { NULL };
	gnutls_datum_t data = { NULL };
	gnutls_pubkey_t pubkey = NULL;
	int sigalg;
	int rc;

	if (resp == NULL || signercert == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	rc = gnutls_ocsp_resp_get_signature_algorithm(resp);
	if (rc < 0) {
		gnutls_assert();
		goto done;
	}
	sigalg = rc;

	rc = _gnutls_x509_get_raw_field2(resp->basicresp, &resp->der, "tbsResponseData", &data);
	if (rc != GNUTLS_E_SUCCESS) {
		gnutls_assert();
		goto done;
	}

	rc = gnutls_pubkey_init(&pubkey);
	if (rc != GNUTLS_E_SUCCESS) {
		gnutls_assert();
		goto done;
	}

	rc = gnutls_pubkey_import_x509(pubkey, signercert, 0);
	if (rc != GNUTLS_E_SUCCESS) {
		gnutls_assert();
		goto done;
	}

	rc = gnutls_ocsp_resp_get_signature(resp, &sig);
	if (rc != GNUTLS_E_SUCCESS) {
		gnutls_assert();
		goto done;
	}

	rc = gnutls_pubkey_verify_data2(pubkey, sigalg, 0, &data, &sig);
	if (rc == GNUTLS_E_PK_SIG_VERIFY_FAILED) {
		gnutls_assert();
		*verify = GNUTLS_OCSP_VERIFY_SIGNATURE_FAILURE;
	} else if (rc < 0) {
		gnutls_assert();
		goto done;
	} else
		*verify = 0;

	rc = GNUTLS_E_SUCCESS;

      done:
	gnutls_free(sig.data);
	gnutls_pubkey_deinit(pubkey);

	return rc;
}