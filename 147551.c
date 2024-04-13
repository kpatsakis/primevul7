cdk_pk_get_mpi(cdk_pubkey_t pk, size_t idx,
	       byte * buf, size_t buflen, size_t * r_nwritten,
	       size_t * r_nbits)
{
	if (!pk || !r_nwritten)
		return CDK_Inv_Value;

	if ((ssize_t) idx > cdk_pk_get_npkey(pk->pubkey_algo))
		return CDK_Inv_Value;
	return mpi_to_buffer(pk->mpi[idx], buf, buflen, r_nwritten,
			     r_nbits);
}