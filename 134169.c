int is_crl_issuer(gnutls_x509_crl_t crl, gnutls_x509_crt_t issuer_cert)
{
    gnutls_datum_t dn1 = { NULL, 0 }, dn2 = {
    NULL, 0};
    int ret;

    ret = _gnutls_x509_crl_get_raw_issuer_dn(crl, &dn1);
    if (ret < 0) {
	gnutls_assert();
	goto cleanup;
    }

    ret = _gnutls_x509_crt_get_raw_dn(issuer_cert, &dn2);
    if (ret < 0) {
	gnutls_assert();
	return ret;
    }

    ret = _gnutls_x509_compare_raw_dn(&dn1, &dn2);

  cleanup:
    _gnutls_free_datum(&dn1);
    _gnutls_free_datum(&dn2);

    return ret;
}