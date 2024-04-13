int gnutls_subject_alt_names_get(gnutls_subject_alt_names_t sans,
				 unsigned int seq, unsigned int *san_type,
				 gnutls_datum_t * san,
				 gnutls_datum_t * othername_oid)
{
	if (seq >= sans->size)
		return gnutls_assert_val(GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE);

	if (san) {
		memcpy(san, &sans->names[seq].san, sizeof(gnutls_datum_t));
	}

	if (san_type)
		*san_type = sans->names[seq].type;

	if (othername_oid != NULL && sans->names[seq].type == GNUTLS_SAN_OTHERNAME) {
		othername_oid->data = sans->names[seq].othername_oid.data;
		othername_oid->size = sans->names[seq].othername_oid.size;
	}

	return 0;
}