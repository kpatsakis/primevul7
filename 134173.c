static int check_if_ca(gnutls_x509_crt_t cert, gnutls_x509_crt_t issuer,
		       unsigned int flags)
{
    gnutls_datum_t cert_signed_data = { NULL, 0 };
    gnutls_datum_t issuer_signed_data = { NULL, 0 };
    gnutls_datum_t cert_signature = { NULL, 0 };
    gnutls_datum_t issuer_signature = { NULL, 0 };
    int result;

    /* Check if the issuer is the same with the
     * certificate. This is added in order for trusted
     * certificates to be able to verify themselves.
     */

    result =
	_gnutls_x509_get_signed_data(issuer->cert, "tbsCertificate",
				     &issuer_signed_data);
    if (result < 0) {
	gnutls_assert();
	goto cleanup;
    }

    result =
	_gnutls_x509_get_signed_data(cert->cert, "tbsCertificate",
				     &cert_signed_data);
    if (result < 0) {
	gnutls_assert();
	goto cleanup;
    }

    result =
	_gnutls_x509_get_signature(issuer->cert, "signature",
				   &issuer_signature);
    if (result < 0) {
	gnutls_assert();
	goto cleanup;
    }

    result =
	_gnutls_x509_get_signature(cert->cert, "signature",
				   &cert_signature);
    if (result < 0) {
	gnutls_assert();
	goto cleanup;
    }

    /* If the subject certificate is the same as the issuer
     * return true.
     */
    if (!(flags & GNUTLS_VERIFY_DO_NOT_ALLOW_SAME))
	if (cert_signed_data.size == issuer_signed_data.size) {
	    if ((memcmp(cert_signed_data.data, issuer_signed_data.data,
			cert_signed_data.size) == 0) &&
		(cert_signature.size == issuer_signature.size) &&
		(memcmp(cert_signature.data, issuer_signature.data,
			cert_signature.size) == 0)) {
		result = 1;
		goto cleanup;
	    }
	}

    if (gnutls_x509_crt_get_ca_status(issuer, NULL) == 1) {
	result = 1;
	goto cleanup;
    } else
	gnutls_assert();

    result = 0;

  cleanup:
    _gnutls_free_datum(&cert_signed_data);
    _gnutls_free_datum(&issuer_signed_data);
    _gnutls_free_datum(&cert_signature);
    _gnutls_free_datum(&issuer_signature);
    return result;
}