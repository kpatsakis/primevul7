gnutls_ocsp_req_add_cert_id(gnutls_ocsp_req_t req,
			    gnutls_digest_algorithm_t digest,
			    const gnutls_datum_t * issuer_name_hash,
			    const gnutls_datum_t * issuer_key_hash,
			    const gnutls_datum_t * serial_number)
{
	int result;
	const char *oid;

	if (req == NULL || issuer_name_hash == NULL
	    || issuer_key_hash == NULL || serial_number == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	oid = _gnutls_x509_digest_to_oid(hash_to_entry(digest));
	if (oid == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	result =
	    asn1_write_value(req->req, "tbsRequest.requestList", "NEW", 1);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	result = asn1_write_value
	    (req->req,
	     "tbsRequest.requestList.?LAST.reqCert.hashAlgorithm.algorithm",
	     oid, 1);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	/* XXX we don't support any algorithm with parameters */
	result = asn1_write_value
	    (req->req,
	     "tbsRequest.requestList.?LAST.reqCert.hashAlgorithm.parameters",
	     ASN1_NULL, ASN1_NULL_SIZE);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	result = asn1_write_value
	    (req->req,
	     "tbsRequest.requestList.?LAST.reqCert.issuerNameHash",
	     issuer_name_hash->data, issuer_name_hash->size);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	result = asn1_write_value
	    (req->req,
	     "tbsRequest.requestList.?LAST.reqCert.issuerKeyHash",
	     issuer_key_hash->data, issuer_key_hash->size);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	result = asn1_write_value
	    (req->req, "tbsRequest.requestList.?LAST.reqCert.serialNumber",
	     serial_number->data, serial_number->size);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	/* XXX add separate function that can add extensions too */
	result = asn1_write_value
	    (req->req,
	     "tbsRequest.requestList.?LAST.singleRequestExtensions", NULL,
	     0);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	return GNUTLS_E_SUCCESS;
}