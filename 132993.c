bighash(
	BIGNUM	*bn,		/* BIGNUM * from */
	BIGNUM	*bk		/* BIGNUM * to */
	)
{
	EVP_MD_CTX ctx;		/* message digest context */
	u_char dgst[EVP_MAX_MD_SIZE]; /* message digest */
	u_char	*ptr;		/* a BIGNUM as binary string */
	u_int	len;

	len = BN_num_bytes(bn);
	ptr = emalloc(len);
	BN_bn2bin(bn, ptr);
	EVP_DigestInit(&ctx, EVP_md5());
	EVP_DigestUpdate(&ctx, ptr, len);
	EVP_DigestFinal(&ctx, dgst, &len);
	BN_bin2bn(dgst, len, bk);
	free(ptr);
}