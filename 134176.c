    gnutls_x509_crt_t find_crl_issuer(gnutls_x509_crl_t crl,
				      const gnutls_x509_crt_t *
				      trusted_cas, int tcas_size)
{
    int i;

    /* this is serial search. 
     */

    for (i = 0; i < tcas_size; i++) {
	if (is_crl_issuer(crl, trusted_cas[i]) == 1)
	    return trusted_cas[i];
    }

    gnutls_assert();
    return NULL;
}