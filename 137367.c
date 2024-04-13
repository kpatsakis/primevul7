int gnutls_x509_crl_dist_points_init(gnutls_x509_crl_dist_points_t * cdp)
{
	*cdp = gnutls_calloc(1, sizeof(struct gnutls_x509_crl_dist_points_st));
	if (*cdp == NULL)
		return gnutls_assert_val(GNUTLS_E_MEMORY_ERROR);

	return 0;
}