cdk_sk_get_mpi(cdk_pkt_seckey_t sk, size_t idx,
	       byte * buf, size_t buflen, size_t * r_nwritten,
	       size_t * r_nbits)
{
	if (!sk || !r_nwritten)
		return CDK_Inv_Value;

	if ((ssize_t) idx > cdk_pk_get_nskey(sk->pubkey_algo))
		return CDK_Inv_Value;
	return mpi_to_buffer(sk->mpi[idx], buf, buflen, r_nwritten,
			     r_nbits);
}