gnutls_ocsp_resp_verify_direct(gnutls_ocsp_resp_t resp,
			       gnutls_x509_crt_t issuer,
			       unsigned int *verify, unsigned int flags)
{
	gnutls_x509_crt_t signercert;
	int rc;

	if (resp == NULL || issuer == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	signercert = find_signercert(resp);
	if (!signercert) {
		signercert = issuer;
	} else if (!gnutls_x509_crt_equals(signercert, issuer)) {

		/* response contains a signer. Verify him */

		unsigned int vtmp;

		rc = gnutls_x509_crt_verify(signercert, &issuer, 1, 0,
					    &vtmp);
		if (rc != GNUTLS_E_SUCCESS) {
			gnutls_assert();
			goto done;
		}

		if (vtmp != 0) {
			*verify = vstatus_to_ocsp_status(vtmp);
			gnutls_assert();
			rc = GNUTLS_E_SUCCESS;
			goto done;
		}

		rc = check_ocsp_purpose(signercert);
		if (rc < 0) {
			gnutls_assert();
			*verify = GNUTLS_OCSP_VERIFY_SIGNER_KEYUSAGE_ERROR;
			rc = GNUTLS_E_SUCCESS;
			goto done;
		}
	}

	rc = _ocsp_resp_verify_direct(resp, signercert, verify, flags);

      done:
	if (signercert != issuer)
		gnutls_x509_crt_deinit(signercert);

	return rc;
}