int gnutls_x509_ext_export_name_constraints(gnutls_x509_name_constraints_t nc,
					 gnutls_datum_t * ext)
{
	int ret, result;
	uint8_t null = 0;
	ASN1_TYPE c2 = ASN1_TYPE_EMPTY;
	struct name_constraints_node_st *tmp;

	if (nc->permitted == NULL && nc->excluded == NULL)
		return gnutls_assert_val(GNUTLS_E_INVALID_REQUEST);

	result = asn1_create_element
	    (_gnutls_get_pkix(), "PKIX1.NameConstraints", &c2);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	if (nc->permitted == NULL) {
		asn1_write_value(c2, "permittedSubtrees", NULL, 0);
	} else {
		tmp = nc->permitted;
		do {
			result =
			    asn1_write_value(c2, "permittedSubtrees", "NEW", 1);
			if (result != ASN1_SUCCESS) {
				gnutls_assert();
				ret = _gnutls_asn2err(result);
				goto cleanup;
			}

			result =
			    asn1_write_value(c2,
					     "permittedSubtrees.?LAST.maximum",
					     NULL, 0);
			if (result != ASN1_SUCCESS) {
				gnutls_assert();
				ret = _gnutls_asn2err(result);
				goto cleanup;
			}

			result =
			    asn1_write_value(c2,
					     "permittedSubtrees.?LAST.minimum",
					     &null, 1);
			if (result != ASN1_SUCCESS) {
				gnutls_assert();
				ret = _gnutls_asn2err(result);
				goto cleanup;
			}

			ret =
			    _gnutls_write_general_name(c2,
						       "permittedSubtrees.?LAST.base",
						       tmp->type,
						       tmp->name.data,
						       tmp->name.size);
			if (ret < 0) {
				gnutls_assert();
				goto cleanup;
			}
			tmp = tmp->next;
		} while (tmp != NULL);
	}

	if (nc->excluded == NULL) {
		asn1_write_value(c2, "excludedSubtrees", NULL, 0);
	} else {
		tmp = nc->excluded;
		do {
			result =
			    asn1_write_value(c2, "excludedSubtrees", "NEW", 1);
			if (result != ASN1_SUCCESS) {
				gnutls_assert();
				ret = _gnutls_asn2err(result);
				goto cleanup;
			}

			result =
			    asn1_write_value(c2,
					     "excludedSubtrees.?LAST.maximum",
					     NULL, 0);
			if (result != ASN1_SUCCESS) {
				gnutls_assert();
				ret = _gnutls_asn2err(result);
				goto cleanup;
			}

			result =
			    asn1_write_value(c2,
					     "excludedSubtrees.?LAST.minimum",
					     &null, 1);
			if (result != ASN1_SUCCESS) {
				gnutls_assert();
				ret = _gnutls_asn2err(result);
				goto cleanup;
			}

			ret =
			    _gnutls_write_general_name(c2,
						       "excludedSubtrees.?LAST.base",
						       tmp->type,
						       tmp->name.data,
						       tmp->name.size);
			if (ret < 0) {
				gnutls_assert();
				goto cleanup;
			}
			tmp = tmp->next;
		} while (tmp != NULL);

	}

	ret = _gnutls_x509_der_encode(c2, "", ext, 0);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	ret = 0;

 cleanup:
	asn1_delete_structure(&c2);
	return ret;
}