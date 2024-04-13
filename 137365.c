void gnutls_x509_aki_deinit(gnutls_x509_aki_t aki)
{
	gnutls_free(aki->serial.data);
	gnutls_free(aki->id.data);
	subject_alt_names_deinit(&aki->cert_issuer);
	gnutls_free(aki);
}