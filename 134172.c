int gnutls_x509_crt_check_issuer(gnutls_x509_crt_t cert,
				 gnutls_x509_crt_t issuer)
{
    return is_issuer(cert, issuer);
}