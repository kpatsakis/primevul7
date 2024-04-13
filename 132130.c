static BOOL security_md5_16_32_32(const BYTE* in0, const BYTE* in1, const BYTE* in2, BYTE* output)
{
	WINPR_DIGEST_CTX* md5 = NULL;
	BOOL result = FALSE;

	if (!(md5 = winpr_Digest_New()))
		return FALSE;

	if (!winpr_Digest_Init(md5, WINPR_MD_MD5))
		goto out;

	if (!winpr_Digest_Update(md5, in0, 16))
		goto out;

	if (!winpr_Digest_Update(md5, in1, 32))
		goto out;

	if (!winpr_Digest_Update(md5, in2, 32))
		goto out;

	if (!winpr_Digest_Final(md5, output, WINPR_MD5_DIGEST_LENGTH))
		goto out;

	result = TRUE;
out:
	winpr_Digest_Free(md5);
	return result;
}