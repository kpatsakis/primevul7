gnutls_ocsp_resp_import(gnutls_ocsp_resp_t resp,
			const gnutls_datum_t * data)
{
	int ret = 0;

	if (resp == NULL || data == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	if (resp->init != 0) {
		/* Any earlier _asn1_strict_der_decode will modify the ASN.1
		   structure, so we need to replace it with a fresh
		   structure. */
		asn1_delete_structure(&resp->resp);
		if (resp->basicresp)
			asn1_delete_structure(&resp->basicresp);

		ret = asn1_create_element(_gnutls_get_pkix(),
					  "PKIX1.OCSPResponse",
					  &resp->resp);
		if (ret != ASN1_SUCCESS) {
			gnutls_assert();
			return _gnutls_asn2err(ret);
		}

		ret = asn1_create_element(_gnutls_get_pkix(),
					  "PKIX1.BasicOCSPResponse",
					  &resp->basicresp);
		if (ret != ASN1_SUCCESS) {
			gnutls_assert();
			return _gnutls_asn2err(ret);
		}

		gnutls_free(resp->der.data);
		resp->der.data = NULL;
	}

	resp->init = 1;
	ret = _asn1_strict_der_decode(&resp->resp, data->data, data->size, NULL);
	if (ret != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(ret);
	}

	if (gnutls_ocsp_resp_get_status(resp) !=
	    GNUTLS_OCSP_RESP_SUCCESSFUL)
		return GNUTLS_E_SUCCESS;

	ret =
	    _gnutls_x509_read_value(resp->resp,
				    "responseBytes.responseType",
				    &resp->response_type_oid);
	if (ret < 0) {
		gnutls_assert();
		return ret;
	}
#define OCSP_BASIC "1.3.6.1.5.5.7.48.1.1"

	if (resp->response_type_oid.size == sizeof(OCSP_BASIC)
	    && memcmp(resp->response_type_oid.data, OCSP_BASIC,
		      resp->response_type_oid.size) == 0) {

		ret =
		    _gnutls_x509_read_value(resp->resp,
					    "responseBytes.response", &resp->der);
		if (ret < 0) {
			gnutls_assert();
			return ret;
		}

		ret =
		    _asn1_strict_der_decode(&resp->basicresp, resp->der.data, resp->der.size,
				      NULL);
		if (ret != ASN1_SUCCESS) {
			gnutls_assert();
			return _gnutls_asn2err(ret);
		}
	} else {
		asn1_delete_structure(&resp->basicresp);
		resp->basicresp = NULL;
	}

	return GNUTLS_E_SUCCESS;
}