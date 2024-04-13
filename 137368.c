int gnutls_x509_policies_init(gnutls_x509_policies_t * policies)
{
	*policies = gnutls_calloc(1, sizeof(struct gnutls_x509_policies_st));
	if (*policies == NULL)
		return gnutls_assert_val(GNUTLS_E_MEMORY_ERROR);

	return 0;
}