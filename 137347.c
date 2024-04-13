int gnutls_x509_crl_dist_points_get(gnutls_x509_crl_dist_points_t cdp,
				    unsigned int seq, unsigned int *type,
				    gnutls_datum_t * san, unsigned int *reasons)
{
	if (seq >= cdp->size)
		return gnutls_assert_val(GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE);

	if (reasons)
		*reasons = cdp->points[seq].reasons;

	if (type)
		*type = cdp->points[seq].type;

	if (san) {
		san->data = cdp->points[seq].san.data;
		san->size = cdp->points[seq].san.size;
	}

	return 0;
}