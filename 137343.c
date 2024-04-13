void gnutls_x509_key_purpose_deinit(gnutls_x509_key_purposes_t p)
{
	key_purposes_deinit(p);
	gnutls_free(p);
}