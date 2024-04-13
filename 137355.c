int gnutls_x509_othername_to_virtual(const char *oid, 
				     const gnutls_datum_t *othername,
				     unsigned int *virt_type,
				     gnutls_datum_t *virt)
{
	int ret;
	unsigned type;

	type = _san_othername_to_virtual(oid, strlen(oid));
	if (type == GNUTLS_SAN_OTHERNAME)
		return gnutls_assert_val(GNUTLS_E_X509_UNKNOWN_SAN);

	if (virt_type)
		*virt_type = type;

	switch(type) {
		case GNUTLS_SAN_OTHERNAME_XMPP:
			ret = _gnutls_x509_decode_string
				    (ASN1_ETYPE_UTF8_STRING, othername->data,
				     othername->size, virt, 0);
			if (ret < 0) {
				gnutls_assert();
				return ret;
			}
			return 0;
		default:
			return gnutls_assert_val(GNUTLS_E_INTERNAL_ERROR);
	}
}