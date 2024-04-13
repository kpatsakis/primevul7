static int verify_ee(const gnutls_datum_t *raw_crt, gnutls_certificate_type_t crt_type,
		 dane_cert_type_t ctype, dane_match_type_t match, gnutls_datum_t * data,
		 unsigned int *verify)
{
gnutls_datum_t pubkey = {NULL, 0};
int ret;

	if (ctype == DANE_CERT_X509 && crt_type == GNUTLS_CRT_X509) {

		if (!matches(raw_crt, data, match)) {
		        gnutls_assert();
			*verify |= DANE_VERIFY_CERT_DIFFERS;
                }

	} else if (ctype == DANE_CERT_PK && crt_type == GNUTLS_CRT_X509) {

		ret = crt_to_pubkey(raw_crt, &pubkey);
		if (ret < 0) {
        	        gnutls_assert();
			goto cleanup;
                }

		if (!matches(&pubkey, data, match)) {
		        gnutls_assert();
			*verify |= DANE_VERIFY_CERT_DIFFERS;
                }
	} else {
		ret = gnutls_assert_val(DANE_E_UNKNOWN_DANE_DATA);
		goto cleanup;
	}

	ret = 0;
cleanup:
	free(pubkey.data);
	return ret;
}