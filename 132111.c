BOOL security_mac_data(const BYTE* mac_salt_key, const BYTE* data, UINT32 length, BYTE* output)
{
	WINPR_DIGEST_CTX* sha1 = NULL;
	WINPR_DIGEST_CTX* md5 = NULL;
	BYTE length_le[4];
	BYTE sha1_digest[WINPR_SHA1_DIGEST_LENGTH];
	BOOL result = FALSE;
	/* MacData = MD5(MacSaltKey + pad2 + SHA1(MacSaltKey + pad1 + length + data)) */
	security_UINT32_le(length_le, length); /* length must be little-endian */

	/* SHA1_Digest = SHA1(MacSaltKey + pad1 + length + data) */
	if (!(sha1 = winpr_Digest_New()))
		goto out;

	if (!winpr_Digest_Init(sha1, WINPR_MD_SHA1))
		goto out;

	if (!winpr_Digest_Update(sha1, mac_salt_key, 16)) /* MacSaltKey */
		goto out;

	if (!winpr_Digest_Update(sha1, pad1, sizeof(pad1))) /* pad1 */
		goto out;

	if (!winpr_Digest_Update(sha1, length_le, sizeof(length_le))) /* length */
		goto out;

	if (!winpr_Digest_Update(sha1, data, length)) /* data */
		goto out;

	if (!winpr_Digest_Final(sha1, sha1_digest, sizeof(sha1_digest)))
		goto out;

	/* MacData = MD5(MacSaltKey + pad2 + SHA1_Digest) */
	if (!(md5 = winpr_Digest_New()))
		goto out;

	/* Allow FIPS override for use of MD5 here, this is only used for creating the MACData field of
	 * the */
	/* Client Platform Challenge Response packet (from MS-RDPELE section 2.2.2.5). This is for RDP
	 * licensing packets */
	/* which will already be encrypted under FIPS, so the use of MD5 here is not for sensitive data
	 * protection. */
	if (!winpr_Digest_Init_Allow_FIPS(md5, WINPR_MD_MD5))
		goto out;

	if (!winpr_Digest_Update(md5, mac_salt_key, 16)) /* MacSaltKey */
		goto out;

	if (!winpr_Digest_Update(md5, pad2, sizeof(pad2))) /* pad2 */
		goto out;

	if (!winpr_Digest_Update(md5, sha1_digest, sizeof(sha1_digest))) /* SHA1_Digest */
		goto out;

	if (!winpr_Digest_Final(md5, output, WINPR_MD5_DIGEST_LENGTH))
		goto out;

	result = TRUE;
out:
	winpr_Digest_Free(sha1);
	winpr_Digest_Free(md5);
	return result;
}