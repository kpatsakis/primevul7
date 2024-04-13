void gnutls_x509_ext_deinit(gnutls_x509_ext_st *ext)
{
	gnutls_free(ext->oid);
	gnutls_free(ext->data.data);
}