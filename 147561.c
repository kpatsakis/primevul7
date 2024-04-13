sig_to_datum(gnutls_datum_t * r_sig, cdk_pkt_signature_t sig)
{
	int err;
	cdk_error_t rc;

	if (!r_sig || !sig)
		return CDK_Inv_Value;

	rc = 0;
	if (is_RSA(sig->pubkey_algo)) {
		err = _gnutls_mpi_dprint(sig->mpi[0], r_sig);
		if (err < 0)
			rc = map_gnutls_error(err);
	} else if (is_DSA(sig->pubkey_algo)) {
		err =
		    _gnutls_encode_ber_rs(r_sig, sig->mpi[0], sig->mpi[1]);
		if (err < 0)
			rc = map_gnutls_error(err);
	} else
		rc = CDK_Inv_Algo;
	return rc;
}