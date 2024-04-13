gnutls_ocsp_resp_get_certs(gnutls_ocsp_resp_t resp,
			   gnutls_x509_crt_t ** certs, size_t * ncerts)
{
	int ret;
	size_t ctr = 0, i;
	gnutls_x509_crt_t *tmpcerts = NULL, *tmpcerts2;
	gnutls_datum_t c = { NULL, 0 };

	if (resp == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	tmpcerts = gnutls_malloc(sizeof(*tmpcerts));
	if (tmpcerts == NULL) {
		gnutls_assert();
		return GNUTLS_E_MEMORY_ERROR;
	}

	for (;;) {
		char name[ASN1_MAX_NAME_SIZE];

		snprintf(name, sizeof(name), "certs.?%u",
			 (unsigned int) (ctr + 1));
		ret =
		    _gnutls_x509_der_encode(resp->basicresp, name, &c, 0);
		if (ret == GNUTLS_E_ASN1_ELEMENT_NOT_FOUND)
			break;
		if (ret != GNUTLS_E_SUCCESS) {
			gnutls_assert();
			goto error;
		}

		tmpcerts2 =
		    gnutls_realloc_fast(tmpcerts,
					(ctr + 2) * sizeof(*tmpcerts));
		if (tmpcerts2 == NULL) {
			gnutls_assert();
			ret = GNUTLS_E_MEMORY_ERROR;
			goto error;
		}
		tmpcerts = tmpcerts2;

		ret = gnutls_x509_crt_init(&tmpcerts[ctr]);
		if (ret != GNUTLS_E_SUCCESS) {
			gnutls_assert();
			goto error;
		}
		ctr++;

		ret = gnutls_x509_crt_import(tmpcerts[ctr - 1], &c,
					     GNUTLS_X509_FMT_DER);
		if (ret != GNUTLS_E_SUCCESS) {
			gnutls_assert();
			goto error;
		}

		gnutls_free(c.data);
		c.data = NULL;
	}

	tmpcerts[ctr] = NULL;

	if (ncerts)
		*ncerts = ctr;
	if (certs)
		*certs = tmpcerts;
	else {
		/* clean up memory */
		ret = GNUTLS_E_SUCCESS;
		goto error;
	}

	return GNUTLS_E_SUCCESS;

      error:
	gnutls_free(c.data);
	for (i = 0; i < ctr; i++)
		gnutls_x509_crt_deinit(tmpcerts[i]);
	gnutls_free(tmpcerts);
	return ret;
}