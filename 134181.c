int gnutls_x509_crt_list_verify(const gnutls_x509_crt_t * cert_list,
    int cert_list_length, const gnutls_x509_crt_t * CA_list,
    int CA_list_length, const gnutls_x509_crl_t * CRL_list,
    int CRL_list_length, unsigned int flags, unsigned int *verify)
{
    if (cert_list == NULL || cert_list_length == 0)
	return GNUTLS_E_NO_CERTIFICATE_FOUND;

    /* Verify certificate 
     */
    *verify =
	_gnutls_x509_verify_certificate(cert_list, cert_list_length,
					CA_list, CA_list_length, CRL_list,
					CRL_list_length, flags);

    return 0;
}