int gnutls_x509_aia_get(gnutls_x509_aia_t aia, unsigned int seq,
			gnutls_datum_t *oid,
			unsigned *san_type,
			gnutls_datum_t *san)
{
	if (seq >= aia->size)
		return gnutls_assert_val(GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE);

	if (san_type)
		*san_type = aia->aia[seq].san_type;
	if (san) {
		san->data = aia->aia[seq].san.data;
		san->size = aia->aia[seq].san.size;
	}

	if (oid) {
		oid->data = aia->aia[seq].oid.data;
		oid->size = aia->aia[seq].oid.size;
	}

	return 0;
}