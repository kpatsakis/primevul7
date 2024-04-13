int gnutls_x509_ext_export_policies(gnutls_x509_policies_t policies,
				 gnutls_datum_t * ext)
{
	int result;
	unsigned i, j;
	gnutls_datum_t der_data, tmpd;
	ASN1_TYPE c2 = ASN1_TYPE_EMPTY;
	const char *oid;

	result =
	    asn1_create_element(_gnutls_get_pkix(),
				"PKIX1.certificatePolicies", &c2);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		result = _gnutls_asn2err(result);
		goto cleanup;
	}

	for (j = 0; j < policies->size; j++) {
		/* 1. write a new policy */
		result = asn1_write_value(c2, "", "NEW", 1);
		if (result != ASN1_SUCCESS) {
			gnutls_assert();
			result = _gnutls_asn2err(result);
			goto cleanup;
		}

		/* 2. Add the OID.
		 */
		result =
		    asn1_write_value(c2, "?LAST.policyIdentifier",
				     policies->policy[j].oid, 1);
		if (result != ASN1_SUCCESS) {
			gnutls_assert();
			result = _gnutls_asn2err(result);
			goto cleanup;
		}

		for (i = 0;
		     i < MIN(policies->policy[j].qualifiers,
			     GNUTLS_MAX_QUALIFIERS); i++) {
			result =
			    asn1_write_value(c2, "?LAST.policyQualifiers",
					     "NEW", 1);
			if (result != ASN1_SUCCESS) {
				gnutls_assert();
				result = _gnutls_asn2err(result);
				goto cleanup;
			}

			if (policies->policy[j].qualifier[i].type ==
			    GNUTLS_X509_QUALIFIER_URI)
				oid = "1.3.6.1.5.5.7.2.1";
			else if (policies->policy[j].qualifier[i].type ==
				 GNUTLS_X509_QUALIFIER_NOTICE)
				oid = "1.3.6.1.5.5.7.2.2";
			else {
				result =
				    gnutls_assert_val(GNUTLS_E_INVALID_REQUEST);
				goto cleanup;
			}

			result =
			    asn1_write_value(c2,
					     "?LAST.policyQualifiers.?LAST.policyQualifierId",
					     oid, 1);
			if (result != ASN1_SUCCESS) {
				gnutls_assert();
				result = _gnutls_asn2err(result);
				goto cleanup;
			}

			if (policies->policy[j].qualifier[i].type ==
			    GNUTLS_X509_QUALIFIER_URI) {
				tmpd.data =
				    (void *)policies->policy[j].qualifier[i].
				    data;
				tmpd.size =
				    policies->policy[j].qualifier[i].size;
				result =
				    _gnutls_x509_write_string(c2,
							      "?LAST.policyQualifiers.?LAST.qualifier",
							      &tmpd,
							      ASN1_ETYPE_IA5_STRING);
				if (result < 0) {
					gnutls_assert();
					goto cleanup;
				}
			} else if (policies->policy[j].qualifier[i].type ==
				   GNUTLS_X509_QUALIFIER_NOTICE) {
				tmpd.data =
				    (void *)policies->policy[j].qualifier[i].
				    data;
				tmpd.size =
				    policies->policy[j].qualifier[i].size;

				if (tmpd.size > 200) {
					gnutls_assert();
					result = GNUTLS_E_INVALID_REQUEST;
					goto cleanup;
				}

				result = encode_user_notice(&tmpd, &der_data);
				if (result < 0) {
					gnutls_assert();
					goto cleanup;
				}

				result =
				    _gnutls_x509_write_value(c2,
							     "?LAST.policyQualifiers.?LAST.qualifier",
							     &der_data);
				_gnutls_free_datum(&der_data);
				if (result < 0) {
					gnutls_assert();
					goto cleanup;
				}
			}
		}
	}

	result = _gnutls_x509_der_encode(c2, "", ext, 0);
	if (result < 0) {
		gnutls_assert();
		goto cleanup;
	}

 cleanup:
	asn1_delete_structure(&c2);

	return result;
}