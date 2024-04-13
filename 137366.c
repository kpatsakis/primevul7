int gnutls_x509_aki_get_id(gnutls_x509_aki_t aki, gnutls_datum_t * id)
{
	if (aki->id.size == 0)
		return gnutls_assert_val(GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE);

	memcpy(id, &aki->id, sizeof(gnutls_datum_t));
	return 0;
}