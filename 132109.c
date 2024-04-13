BOOL security_fips_check_signature(const BYTE* data, size_t length, const BYTE* sig, rdpRdp* rdp)
{
	BYTE buf[WINPR_SHA1_DIGEST_LENGTH];
	BYTE use_count_le[4];
	WINPR_HMAC_CTX* hmac;
	BOOL result = FALSE;
	security_UINT32_le(use_count_le, rdp->decrypt_use_count);

	if (!(hmac = winpr_HMAC_New()))
		return FALSE;

	if (!winpr_HMAC_Init(hmac, WINPR_MD_SHA1, rdp->fips_sign_key, WINPR_SHA1_DIGEST_LENGTH))
		goto out;

	if (!winpr_HMAC_Update(hmac, data, length))
		goto out;

	if (!winpr_HMAC_Update(hmac, use_count_le, 4))
		goto out;

	if (!winpr_HMAC_Final(hmac, buf, WINPR_SHA1_DIGEST_LENGTH))
		goto out;

	rdp->decrypt_use_count++;

	if (!memcmp(sig, buf, 8))
		result = TRUE;

out:
	winpr_HMAC_Free(hmac);
	return result;
}