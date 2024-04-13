BOOL security_salted_mac_signature(rdpRdp* rdp, const BYTE* data, UINT32 length, BOOL encryption,
                                   BYTE* output)
{
	WINPR_DIGEST_CTX* sha1 = NULL;
	WINPR_DIGEST_CTX* md5 = NULL;
	BYTE length_le[4];
	BYTE use_count_le[4];
	BYTE md5_digest[WINPR_MD5_DIGEST_LENGTH];
	BYTE sha1_digest[WINPR_SHA1_DIGEST_LENGTH];
	BOOL result = FALSE;
	security_UINT32_le(length_le, length); /* length must be little-endian */

	if (encryption)
	{
		security_UINT32_le(use_count_le, rdp->encrypt_checksum_use_count);
	}
	else
	{
		/*
		 * We calculate checksum on plain text, so we must have already
		 * decrypt it, which means decrypt_checksum_use_count is off by one.
		 */
		security_UINT32_le(use_count_le, rdp->decrypt_checksum_use_count - 1);
	}

	/* SHA1_Digest = SHA1(MACKeyN + pad1 + length + data) */
	if (!(sha1 = winpr_Digest_New()))
		goto out;

	if (!winpr_Digest_Init(sha1, WINPR_MD_SHA1))
		goto out;

	if (!winpr_Digest_Update(sha1, rdp->sign_key, rdp->rc4_key_len)) /* MacKeyN */
		goto out;

	if (!winpr_Digest_Update(sha1, pad1, sizeof(pad1))) /* pad1 */
		goto out;

	if (!winpr_Digest_Update(sha1, length_le, sizeof(length_le))) /* length */
		goto out;

	if (!winpr_Digest_Update(sha1, data, length)) /* data */
		goto out;

	if (!winpr_Digest_Update(sha1, use_count_le, sizeof(use_count_le))) /* encryptionCount */
		goto out;

	if (!winpr_Digest_Final(sha1, sha1_digest, sizeof(sha1_digest)))
		goto out;

	/* MACSignature = First64Bits(MD5(MACKeyN + pad2 + SHA1_Digest)) */
	if (!(md5 = winpr_Digest_New()))
		goto out;

	if (!winpr_Digest_Init(md5, WINPR_MD_MD5))
		goto out;

	if (!winpr_Digest_Update(md5, rdp->sign_key, rdp->rc4_key_len)) /* MacKeyN */
		goto out;

	if (!winpr_Digest_Update(md5, pad2, sizeof(pad2))) /* pad2 */
		goto out;

	if (!winpr_Digest_Update(md5, sha1_digest, sizeof(sha1_digest))) /* SHA1_Digest */
		goto out;

	if (!winpr_Digest_Final(md5, md5_digest, sizeof(md5_digest)))
		goto out;

	memcpy(output, md5_digest, 8);
	result = TRUE;
out:
	winpr_Digest_Free(sha1);
	winpr_Digest_Free(md5);
	return result;
}