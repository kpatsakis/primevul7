gnutls_ocsp_resp_get_responder_raw_id(gnutls_ocsp_resp_t resp,
				      unsigned type,
				      gnutls_datum_t * raw)
{
	int ret;

	if (resp == NULL || raw == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	if (type == GNUTLS_OCSP_RESP_ID_KEY)
		ret = _gnutls_x509_read_value(resp->basicresp, "tbsResponseData.responderID.byKey", raw);
	else {
		gnutls_datum_t tmp;

		/* simply reading a CHOICE of CHOICE value doesn't work in libtasn1 */
		ret = _gnutls_x509_get_raw_field2(resp->basicresp, &resp->der,
					  "tbsResponseData.responderID.byName",
					  &tmp);
		if (ret >= 0) {
			int real;
			/* skip the tag */
			if (tmp.size < 2) {
				gnutls_assert();
				ret = GNUTLS_E_ASN1_GENERIC_ERROR;
				goto fail;
			}

			tmp.data++;
			tmp.size--;

			ret = asn1_get_length_der(tmp.data, tmp.size, &real);
			if (ret < 0) {
				gnutls_assert();
				ret = GNUTLS_E_ASN1_GENERIC_ERROR;
				goto fail;
			}

			if (tmp.size < (unsigned)real) {
				gnutls_assert();
				ret = GNUTLS_E_ASN1_GENERIC_ERROR;
				goto fail;
			}

			tmp.data+=real;
			tmp.size-=real;

			ret = _gnutls_set_datum(raw, tmp.data, tmp.size);
		}
	}

	if (ret == GNUTLS_E_ASN1_ELEMENT_NOT_FOUND || ret == GNUTLS_E_ASN1_VALUE_NOT_FOUND)
		return GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE;

 fail:
	return ret;
}