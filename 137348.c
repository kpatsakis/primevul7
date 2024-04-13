int gnutls_x509_aki_init(gnutls_x509_aki_t * aki)
{
	*aki = gnutls_calloc(1, sizeof(struct gnutls_x509_aki_st));
	if (*aki == NULL)
		return gnutls_assert_val(GNUTLS_E_MEMORY_ERROR);

	return 0;
}