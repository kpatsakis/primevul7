cdk_pk_from_secret_key(cdk_pkt_seckey_t sk, cdk_pubkey_t * ret_pk)
{
	if (!sk)
		return CDK_Inv_Value;
	return _cdk_copy_pubkey(ret_pk, sk->pk);
}