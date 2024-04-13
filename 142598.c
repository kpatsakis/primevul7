gnutls_ocsp_req_get_cert_id(gnutls_ocsp_req_t req,
			    unsigned indx,
			    gnutls_digest_algorithm_t * digest,
			    gnutls_datum_t * issuer_name_hash,
			    gnutls_datum_t * issuer_key_hash,
			    gnutls_datum_t * serial_number)
{
	gnutls_datum_t sa;
	char name[ASN1_MAX_NAME_SIZE];
	int ret;

	if (req == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	snprintf(name, sizeof(name),
		 "tbsRequest.requestList.?%u.reqCert.hashAlgorithm.algorithm",
		 indx + 1);
	ret = _gnutls_x509_read_value(req->req, name, &sa);
	if (ret == GNUTLS_E_ASN1_ELEMENT_NOT_FOUND)
		return GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE;
	else if (ret < 0) {
		gnutls_assert();
		return ret;
	}

	ret = gnutls_oid_to_digest((char *) sa.data);
	_gnutls_free_datum(&sa);
	if (ret < 0) {
		gnutls_assert();
		return ret;
	}

	if (digest)
		*digest = ret;

	if (issuer_name_hash) {
		snprintf(name, sizeof(name),
			 "tbsRequest.requestList.?%u.reqCert.issuerNameHash",
			 indx + 1);
		ret =
		    _gnutls_x509_read_value(req->req, name,
					    issuer_name_hash);
		if (ret != GNUTLS_E_SUCCESS) {
			gnutls_assert();
			return ret;
		}
	}

	if (issuer_key_hash) {
		snprintf(name, sizeof(name),
			 "tbsRequest.requestList.?%u.reqCert.issuerKeyHash",
			 indx + 1);
		ret =
		    _gnutls_x509_read_value(req->req, name,
					    issuer_key_hash);
		if (ret != GNUTLS_E_SUCCESS) {
			gnutls_assert();
			if (issuer_name_hash)
				gnutls_free(issuer_name_hash->data);
			return ret;
		}
	}

	if (serial_number) {
		snprintf(name, sizeof(name),
			 "tbsRequest.requestList.?%u.reqCert.serialNumber",
			 indx + 1);
		ret =
		    _gnutls_x509_read_value(req->req, name, serial_number);
		if (ret != GNUTLS_E_SUCCESS) {
			gnutls_assert();
			if (issuer_name_hash)
				gnutls_free(issuer_name_hash->data);
			if (issuer_key_hash)
				gnutls_free(issuer_key_hash->data);
			return ret;
		}
	}

	return GNUTLS_E_SUCCESS;
}