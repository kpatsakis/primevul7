gnutls_ocsp_req_import(gnutls_ocsp_req_t req, const gnutls_datum_t * data)
{
	int ret = 0;

	if (req == NULL || data == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	if (req->init) {
		/* Any earlier _asn1_strict_der_decode will modify the ASN.1
		   structure, so we need to replace it with a fresh
		   structure. */
		asn1_delete_structure(&req->req);

		ret = asn1_create_element(_gnutls_get_pkix(),
					  "PKIX1.OCSPRequest", &req->req);
		if (ret != ASN1_SUCCESS) {
			gnutls_assert();
			return _gnutls_asn2err(ret);
		}
	}
	req->init = 1;

	ret = _asn1_strict_der_decode(&req->req, data->data, data->size, NULL);
	if (ret != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(ret);
	}

	return GNUTLS_E_SUCCESS;
}