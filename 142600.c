gnutls_ocsp_req_set_extension(gnutls_ocsp_req_t req,
			      const char *oid,
			      unsigned int critical,
			      const gnutls_datum_t * data)
{
	if (req == NULL || oid == NULL || data == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	return _gnutls_set_extension(req->req, "tbsRequest.requestExtensions", oid,
			     data, critical);
}