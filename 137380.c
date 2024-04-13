int gnutls_x509_key_purpose_get(gnutls_x509_key_purposes_t p, unsigned idx, gnutls_datum_t *oid)
{
	if (idx >= p->size)
		return gnutls_assert_val(GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE);

	oid->data = p->oid[idx].data;
	oid->size = p->oid[idx].size;

	return 0;
}