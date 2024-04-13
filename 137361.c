int gnutls_x509_key_purpose_init(gnutls_x509_key_purposes_t * p)
{
	*p = gnutls_calloc(1, sizeof(struct gnutls_x509_key_purposes_st));
	if (*p == NULL) {
		gnutls_assert();
		return GNUTLS_E_MEMORY_ERROR;
	}

	return 0;
}