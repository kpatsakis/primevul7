int gnutls_subject_alt_names_init(gnutls_subject_alt_names_t * sans)
{
	*sans = gnutls_calloc(1, sizeof(struct gnutls_subject_alt_names_st));
	if (*sans == NULL) {
		gnutls_assert();
		return GNUTLS_E_MEMORY_ERROR;
	}

	return 0;
}