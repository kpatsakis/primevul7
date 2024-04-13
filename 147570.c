cdk_pk_to_fingerprint(cdk_pubkey_t pk,
		      byte * fprbuf, size_t fprbuflen, size_t * r_nout)
{
	size_t key_fprlen;
	cdk_error_t err;

	if (!pk)
		return CDK_Inv_Value;

	if (pk->version < 4)
		key_fprlen = 16;
	else
		key_fprlen = 20;

	/* Only return the required buffer size for the fingerprint. */
	if (!fprbuf && !fprbuflen && r_nout) {
		*r_nout = key_fprlen;
		return 0;
	}

	if (!fprbuf || key_fprlen > fprbuflen)
		return CDK_Too_Short;

	err = cdk_pk_get_fingerprint(pk, fprbuf);
	if (r_nout)
		*r_nout = key_fprlen;

	return err;
}