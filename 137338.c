void gnutls_subject_alt_names_deinit(gnutls_subject_alt_names_t sans)
{
	subject_alt_names_deinit(sans);
	gnutls_free(sans);
}