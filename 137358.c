int gnutls_x509_crl_dist_points_set(gnutls_x509_crl_dist_points_t cdp,
				    gnutls_x509_subject_alt_name_t type,
				    const gnutls_datum_t * san,
				    unsigned int reasons)
{
	int ret;
	gnutls_datum_t t_san;

	ret = _gnutls_set_datum(&t_san, san->data, san->size);
	if (ret < 0)
		return gnutls_assert_val(ret);

	ret = crl_dist_points_set(cdp, type, &t_san, reasons);
	if (ret < 0) {
		gnutls_free(t_san.data);
		return gnutls_assert_val(ret);
	}

	return 0;
}