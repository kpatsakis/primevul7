int gnutls_x509_crl_verify(gnutls_x509_crl_t crl,
			   const gnutls_x509_crt_t * CA_list,
			   int CA_list_length, unsigned int flags,
			   unsigned int *verify)
{
    int ret;
    /* Verify crl 
     */
    ret = _gnutls_verify_crl2(crl, CA_list, CA_list_length, flags, verify);
    if (ret < 0) {
	gnutls_assert();
	return ret;
    }

    return 0;
}