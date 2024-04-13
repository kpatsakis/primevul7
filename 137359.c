void gnutls_x509_aia_deinit(gnutls_x509_aia_t aia)
{
	unsigned i;

	for (i = 0; i < aia->size; i++) {
		gnutls_free(aia->aia[i].san.data);
		gnutls_free(aia->aia[i].oid.data);
	}
	gnutls_free(aia->aia);
	gnutls_free(aia);
}