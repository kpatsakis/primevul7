gnutls_ocsp_resp_verify(gnutls_ocsp_resp_t resp,
			gnutls_x509_trust_list_t trustlist,
			unsigned int *verify, unsigned int flags)
{
	gnutls_x509_crt_t signercert = NULL;
	int rc;

	/* Algorithm:
	   1. Find signer cert.
	   1a. Search in OCSP response Certificate field for responderID.
	   1b. Verify that signer cert is trusted.
	   2a. It is in trustlist?
	   2b. It has OCSP key usage and directly signed by a CA in trustlist?
	   3. Verify signature of Basic Response using public key from signer cert.
	 */

	signercert = find_signercert(resp);
	if (!signercert) {
		gnutls_datum_t dn;

		rc = gnutls_ocsp_resp_get_responder_raw_id(resp, GNUTLS_OCSP_RESP_ID_DN, &dn);
		if (rc < 0) {
			gnutls_assert();
			*verify = GNUTLS_OCSP_VERIFY_SIGNER_NOT_FOUND;
			rc = GNUTLS_E_SUCCESS;
			goto done;
		}

		rc = gnutls_x509_trust_list_get_issuer_by_dn(trustlist, &dn, &signercert, 0);
		gnutls_free(dn.data);

		if (rc < 0) {
			gnutls_assert();
			*verify = GNUTLS_OCSP_VERIFY_SIGNER_NOT_FOUND;
			rc = GNUTLS_E_SUCCESS;
			goto done;
		}
	} else {
		/* Either the signer is directly trusted (i.e., in trustlist) or it
		   is directly signed by something in trustlist and has proper OCSP
		   extkeyusage. */
		rc = _gnutls_trustlist_inlist(trustlist, signercert);
		if (rc == 0) {
			/* not in trustlist, need to verify signature and bits */
			unsigned vtmp;
			gnutls_typed_vdata_st vdata;

			vdata.type = GNUTLS_DT_KEY_PURPOSE_OID;
			vdata.data = (void*)GNUTLS_KP_OCSP_SIGNING;
			vdata.size = 0;

			gnutls_assert();

			rc = gnutls_x509_trust_list_verify_crt2(trustlist,
								&signercert, 1,
								&vdata, 1,
								0, &vtmp, NULL);
			if (rc != GNUTLS_E_SUCCESS) {
				gnutls_assert();
				goto done;
			}

			if (vtmp != 0) {
				*verify = vstatus_to_ocsp_status(vtmp);
				gnutls_assert();
				rc = GNUTLS_E_SUCCESS;
				goto done;
			}
		}
	}

	rc = check_ocsp_purpose(signercert);
	if (rc < 0) {
		gnutls_assert();
		*verify = GNUTLS_OCSP_VERIFY_SIGNER_KEYUSAGE_ERROR;
		rc = GNUTLS_E_SUCCESS;
		goto done;
	}

	rc = _ocsp_resp_verify_direct(resp, signercert, verify, flags);

      done:
	gnutls_x509_crt_deinit(signercert);

	return rc;
}