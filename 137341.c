int gnutls_x509_key_purpose_set(gnutls_x509_key_purposes_t p, const char *oid)
{
	if (p->size + 1 > MAX_ENTRIES)
		return gnutls_assert_val(GNUTLS_E_INVALID_REQUEST);

	p->oid[p->size].data = (void*)gnutls_strdup(oid);
	if (p->oid[p->size].data == NULL)
		return gnutls_assert_val(GNUTLS_E_MEMORY_ERROR);

	p->oid[p->size].size = strlen(oid);
	p->size++;

	return 0;
}