int gnutls_x509_aia_init(gnutls_x509_aia_t * aia)
{
	*aia = gnutls_calloc(1, sizeof(struct gnutls_x509_aia_st));
	if (*aia == NULL)
		return gnutls_assert_val(GNUTLS_E_MEMORY_ERROR);

	return 0;
}