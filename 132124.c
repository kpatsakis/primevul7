static BOOL security_key_update(BYTE* key, BYTE* update_key, int key_len, rdpRdp* rdp)
{
	BYTE sha1h[WINPR_SHA1_DIGEST_LENGTH];
	WINPR_DIGEST_CTX* sha1 = NULL;
	WINPR_DIGEST_CTX* md5 = NULL;
	WINPR_RC4_CTX* rc4 = NULL;
	BYTE salt[] = { 0xD1, 0x26, 0x9E }; /* 40 bits: 3 bytes, 56 bits: 1 byte */
	BOOL result = FALSE;
	WLog_DBG(TAG, "updating RDP key");

	if (!(sha1 = winpr_Digest_New()))
		goto out;

	if (!winpr_Digest_Init(sha1, WINPR_MD_SHA1))
		goto out;

	if (!winpr_Digest_Update(sha1, update_key, key_len))
		goto out;

	if (!winpr_Digest_Update(sha1, pad1, sizeof(pad1)))
		goto out;

	if (!winpr_Digest_Update(sha1, key, key_len))
		goto out;

	if (!winpr_Digest_Final(sha1, sha1h, sizeof(sha1h)))
		goto out;

	if (!(md5 = winpr_Digest_New()))
		goto out;

	if (!winpr_Digest_Init(md5, WINPR_MD_MD5))
		goto out;

	if (!winpr_Digest_Update(md5, update_key, key_len))
		goto out;

	if (!winpr_Digest_Update(md5, pad2, sizeof(pad2)))
		goto out;

	if (!winpr_Digest_Update(md5, sha1h, sizeof(sha1h)))
		goto out;

	if (!winpr_Digest_Final(md5, key, WINPR_MD5_DIGEST_LENGTH))
		goto out;

	if (!(rc4 = winpr_RC4_New(key, key_len)))
		goto out;

	if (!winpr_RC4_Update(rc4, key_len, key, key))
		goto out;

	if (rdp->settings->EncryptionMethods == ENCRYPTION_METHOD_40BIT)
		memcpy(key, salt, 3);
	else if (rdp->settings->EncryptionMethods == ENCRYPTION_METHOD_56BIT)
		memcpy(key, salt, 1);

	result = TRUE;
out:
	winpr_Digest_Free(sha1);
	winpr_Digest_Free(md5);
	winpr_RC4_Free(rc4);
	return result;
}