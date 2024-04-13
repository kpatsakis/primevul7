cdk_pk_verify(cdk_pubkey_t pk, cdk_pkt_signature_t sig, const byte * md)
{
	gnutls_datum_t s_sig = { NULL, 0 }, di = {
	NULL, 0};
	byte *encmd = NULL;
	cdk_error_t rc;
	int ret, algo;
	unsigned int i;
	gnutls_pk_params_st params;
	const mac_entry_st *me;

	if (!pk || !sig || !md) {
		gnutls_assert();
		return CDK_Inv_Value;
	}

	if (is_DSA(pk->pubkey_algo))
		algo = GNUTLS_PK_DSA;
	else if (is_RSA(pk->pubkey_algo))
		algo = GNUTLS_PK_RSA;
	else {
		gnutls_assert();
		return CDK_Inv_Value;
	}

	rc = sig_to_datum(&s_sig, sig);
	if (rc) {
		gnutls_assert();
		goto leave;
	}

	me = mac_to_entry(sig->digest_algo);
	rc = _gnutls_set_datum(&di, md, _gnutls_hash_get_algo_len(me));
	if (rc < 0) {
		rc = gnutls_assert_val(CDK_Out_Of_Core);
		goto leave;
	}

	rc = pk_prepare_hash(algo, me, &di);
	if (rc < 0) {
		rc = gnutls_assert_val(CDK_General_Error);
		goto leave;
	}

	params.params_nr = cdk_pk_get_npkey(pk->pubkey_algo);
	for (i = 0; i < params.params_nr; i++)
		params.params[i] = pk->mpi[i];
	params.flags = 0;
	ret = _gnutls_pk_verify(algo, &di, &s_sig, &params);

	if (ret < 0) {
		gnutls_assert();
		rc = map_gnutls_error(ret);
		goto leave;
	}

	rc = 0;

      leave:
	_gnutls_free_datum(&s_sig);
	_gnutls_free_datum(&di);
	cdk_free(encmd);
	return rc;
}