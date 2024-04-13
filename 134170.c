int gnutls_x509_crl_check_issuer(gnutls_x509_crl_t cert,
				 gnutls_x509_crt_t issuer)
{
    return is_crl_issuer(cert, issuer);
}