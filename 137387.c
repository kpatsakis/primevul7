static void subject_alt_names_deinit(gnutls_subject_alt_names_t sans)
{
	unsigned int i;

	for (i = 0; i < sans->size; i++) {
		gnutls_free(sans->names[i].san.data);
		gnutls_free(sans->names[i].othername_oid.data);
	}
	gnutls_free(sans->names);
}