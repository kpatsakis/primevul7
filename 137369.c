void gnutls_x509_crl_dist_points_deinit(gnutls_x509_crl_dist_points_t cdp)
{
	unsigned i;

	for (i = 0; i < cdp->size; i++) {
		gnutls_free(cdp->points[i].san.data);
	}
	gnutls_free(cdp->points);
	gnutls_free(cdp);
}