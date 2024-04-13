int crl_dist_points_set(gnutls_x509_crl_dist_points_t cdp,
			gnutls_x509_subject_alt_name_t type,
			const gnutls_datum_t * san, unsigned int reasons)
{
	void *tmp;

	/* new dist point */
	tmp =
	    gnutls_realloc(cdp->points,
			   (cdp->size + 1) * sizeof(cdp->points[0]));
	if (tmp == NULL) {
		return gnutls_assert_val(GNUTLS_E_MEMORY_ERROR);
	}
	cdp->points = tmp;

	cdp->points[cdp->size].type = type;
	cdp->points[cdp->size].san.data = san->data;
	cdp->points[cdp->size].san.size = san->size;
	cdp->points[cdp->size].reasons = reasons;

	cdp->size++;
	return 0;

}