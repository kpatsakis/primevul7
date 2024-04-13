static void key_purposes_deinit(gnutls_x509_key_purposes_t p)
{
	unsigned int i;

	for (i = 0; i < p->size; i++) {
		gnutls_free(p->oid[i].data);
	}
}