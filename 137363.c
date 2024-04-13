int gnutls_x509_ext_export_crl_dist_points(gnutls_x509_crl_dist_points_t cdp,
					gnutls_datum_t * ext)
{
	ASN1_TYPE c2 = ASN1_TYPE_EMPTY;
	int result;
	uint8_t reasons[2];
	unsigned i;

	result =
	    asn1_create_element(_gnutls_get_pkix(),
				"PKIX1.CRLDistributionPoints", &c2);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		result = _gnutls_asn2err(result);
		goto cleanup;
	}

	for (i = 0; i < cdp->size; i++) {

		if (i == 0
		    || cdp->points[i].reasons != cdp->points[i - 1].reasons) {
			result = asn1_write_value(c2, "", "NEW", 1);
			if (result != ASN1_SUCCESS) {
				gnutls_assert();
				result = _gnutls_asn2err(result);
				goto cleanup;
			}

			if (cdp->points[i].reasons) {
				reasons[0] = cdp->points[i].reasons & 0xff;
				reasons[1] = cdp->points[i].reasons >> 8;

				result =
				    asn1_write_value(c2, "?LAST.reasons",
						     reasons, 2);
			} else {
				result =
				    asn1_write_value(c2, "?LAST.reasons", NULL,
						     0);
			}

			if (result != ASN1_SUCCESS) {
				gnutls_assert();
				result = _gnutls_asn2err(result);
				goto cleanup;
			}

			result =
			    asn1_write_value(c2, "?LAST.cRLIssuer", NULL, 0);
			if (result != ASN1_SUCCESS) {
				gnutls_assert();
				result = _gnutls_asn2err(result);
				goto cleanup;
			}
			/* When used as type CHOICE.
			 */
			result =
			    asn1_write_value(c2, "?LAST.distributionPoint",
					     "fullName", 1);
			if (result != ASN1_SUCCESS) {
				gnutls_assert();
				result = _gnutls_asn2err(result);
				goto cleanup;
			}
		}

		result =
		    _gnutls_write_new_general_name(c2,
						   "?LAST.distributionPoint.fullName",
						   cdp->points[i].type,
						   cdp->points[i].san.data,
						   cdp->points[i].san.size);
		if (result < 0) {
			gnutls_assert();
			goto cleanup;
		}
	}

	result = _gnutls_x509_der_encode(c2, "", ext, 0);
	if (result < 0) {
		gnutls_assert();
		goto cleanup;
	}

	result = 0;

 cleanup:
	asn1_delete_structure(&c2);

	return result;

}