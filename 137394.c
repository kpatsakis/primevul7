int gnutls_x509_aki_set_id(gnutls_x509_aki_t aki, const gnutls_datum_t * id)
{
	return _gnutls_set_datum(&aki->id, id->data, id->size);
}