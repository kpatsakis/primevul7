gnutls_ocsp_resp_get_single(gnutls_ocsp_resp_t resp,
			    unsigned indx,
			    gnutls_digest_algorithm_t * digest,
			    gnutls_datum_t * issuer_name_hash,
			    gnutls_datum_t * issuer_key_hash,
			    gnutls_datum_t * serial_number,
			    unsigned int *cert_status,
			    time_t * this_update,
			    time_t * next_update,
			    time_t * revocation_time,
			    unsigned int *revocation_reason)
{
	gnutls_datum_t sa;
	char name[ASN1_MAX_NAME_SIZE];
	int ret;

	snprintf(name, sizeof(name),
		 "tbsResponseData.responses.?%u.certID.hashAlgorithm.algorithm",
		 indx + 1);
	ret = _gnutls_x509_read_value(resp->basicresp, name, &sa);
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
			 "tbsResponseData.responses.?%u.certID.issuerNameHash",
			 indx + 1);
		ret = _gnutls_x509_read_value(resp->basicresp, name,
					      issuer_name_hash);
		if (ret != GNUTLS_E_SUCCESS) {
			gnutls_assert();
			return ret;
		}
	}

	if (issuer_key_hash) {
		snprintf(name, sizeof(name),
			 "tbsResponseData.responses.?%u.certID.issuerKeyHash",
			 indx + 1);
		ret = _gnutls_x509_read_value(resp->basicresp, name,
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
			 "tbsResponseData.responses.?%u.certID.serialNumber",
			 indx + 1);
		ret = _gnutls_x509_read_value(resp->basicresp, name,
					      serial_number);
		if (ret != GNUTLS_E_SUCCESS) {
			gnutls_assert();
			if (issuer_name_hash)
				gnutls_free(issuer_name_hash->data);
			if (issuer_key_hash)
				gnutls_free(issuer_key_hash->data);
			return ret;
		}
	}

	if (cert_status) {
		snprintf(name, sizeof(name),
			 "tbsResponseData.responses.?%u.certStatus",
			 indx + 1);
		ret = _gnutls_x509_read_value(resp->basicresp, name, &sa);
		if (ret == GNUTLS_E_ASN1_ELEMENT_NOT_FOUND)
			return GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE;
		else if (ret < 0) {
			gnutls_assert();
			return ret;
		}
		if (sa.size == 5 && memcmp(sa.data, "good", sa.size) == 0)
			*cert_status = GNUTLS_OCSP_CERT_GOOD;
		else if (sa.size == 8
			 && memcmp(sa.data, "revoked", sa.size) == 0)
			*cert_status = GNUTLS_OCSP_CERT_REVOKED;
		else if (sa.size == 8
			 && memcmp(sa.data, "unknown", sa.size) == 0)
			*cert_status = GNUTLS_OCSP_CERT_UNKNOWN;
		else {
			gnutls_assert();
			gnutls_free(sa.data);
			return GNUTLS_E_ASN1_DER_ERROR;
		}
		gnutls_free(sa.data);
	}

	if (this_update) {
		char ttime[MAX_TIME];
		int len;

		snprintf(name, sizeof(name),
			 "tbsResponseData.responses.?%u.thisUpdate",
			 indx + 1);
		len = sizeof(ttime) - 1;
		ret = asn1_read_value(resp->basicresp, name, ttime, &len);
		if (ret != ASN1_SUCCESS) {
			gnutls_assert();
			return GNUTLS_E_ASN1_DER_ERROR;
		} else {
			*this_update =
			    _gnutls_x509_generalTime2gtime(ttime);
		}
	}

	if (next_update) {
		char ttime[MAX_TIME];
		int len;

		snprintf(name, sizeof(name),
			 "tbsResponseData.responses.?%u.nextUpdate",
			 indx + 1);
		len = sizeof(ttime) - 1;
		ret = asn1_read_value(resp->basicresp, name, ttime, &len);
		if (ret != ASN1_SUCCESS) {
			gnutls_assert();
			*next_update = (time_t) (-1);
		} else
			*next_update =
			    _gnutls_x509_generalTime2gtime(ttime);
	}

	if (revocation_time) {
		char ttime[MAX_TIME];
		int len;

		snprintf(name, sizeof(name),
			 "tbsResponseData.responses.?%u.certStatus."
			 "revoked.revocationTime", indx + 1);
		len = sizeof(ttime) - 1;
		ret = asn1_read_value(resp->basicresp, name, ttime, &len);
		if (ret != ASN1_SUCCESS) {
			gnutls_assert();
			*revocation_time = (time_t) (-1);
		} else
			*revocation_time =
			    _gnutls_x509_generalTime2gtime(ttime);
	}

	/* revocation_reason */
	if (revocation_reason) {
		snprintf(name, sizeof(name),
			 "tbsResponseData.responses.?%u.certStatus."
			 "revoked.revocationReason", indx + 1);

		ret = _gnutls_x509_read_uint(resp->basicresp, name,
					     revocation_reason);
		if (ret < 0)
			*revocation_reason =
			    GNUTLS_X509_CRLREASON_UNSPECIFIED;
	}

	return GNUTLS_E_SUCCESS;
}