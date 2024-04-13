unsigned int _gnutls_x509_verify_certificate(
    const gnutls_x509_crt_t *certificate_list, int clist_size,
    const gnutls_x509_crt_t *trusted_cas, int tcas_size,
    const gnutls_x509_crl_t *CRLs, int crls_size,
    unsigned int flags)
{
    int i = 0, ret;
    unsigned int status = 0, output;


    /* Verify the last certificate in the certificate path
     * against the trusted CA certificate list.
     *
     * If no CAs are present returns NOT_TRUSTED. Thus works
     * in self signed etc certificates.
     */
    ret =
	_gnutls_verify_certificate2(certificate_list[clist_size-1], 
	    trusted_cas, tcas_size, flags, &output);

    if (ret == 0) {
	/* if the last certificate in the certificate
	 * list is invalid, then the certificate is not
	 * trusted.
	 */
	gnutls_assert();
	status |= output;
	status |= GNUTLS_CERT_INVALID;
	return status;
    }

    /* Check for revoked certificates in the chain
     */
#ifdef ENABLE_PKI
    for (i = 0; i < clist_size; i++) {
	ret = gnutls_x509_crt_check_revocation(certificate_list[i],
					       CRLs, crls_size);
	if (ret == 1) {		/* revoked */
	    status |= GNUTLS_CERT_REVOKED;
	    status |= GNUTLS_CERT_INVALID;
	    return status;
	}
    }
#endif

    /* Verify the certificate path (chain) 
     */
    for (i = clist_size-1; i > 0; i--) {
	if (i - 1 < 0)
	    break;

        /* note that here we disable this V1 CA flag. So that no version 1
         * certificates can exist in a supplied chain.
         */
        flags ^= GNUTLS_VERIFY_ALLOW_X509_V1_CA_CRT;
	if ((ret =
	     _gnutls_verify_certificate2(certificate_list[i-1],
                  &certificate_list[i], 1, flags, NULL)) == 0) {
	    status |= GNUTLS_CERT_INVALID;
	    return status;
	}
    }

    return 0;
}