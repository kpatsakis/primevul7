BOOL security_encrypt(BYTE* data, size_t length, rdpRdp* rdp)
{
	BOOL rc = FALSE;
	EnterCriticalSection(&rdp->critical);
	if (rdp->encrypt_use_count >= 4096)
	{
		if (!security_key_update(rdp->encrypt_key, rdp->encrypt_update_key, rdp->rc4_key_len, rdp))
			goto fail;

		winpr_RC4_Free(rdp->rc4_encrypt_key);
		rdp->rc4_encrypt_key = winpr_RC4_New(rdp->encrypt_key, rdp->rc4_key_len);

		if (!rdp->rc4_encrypt_key)
			goto fail;

		rdp->encrypt_use_count = 0;
	}

	if (!winpr_RC4_Update(rdp->rc4_encrypt_key, length, data, data))
		goto fail;

	rdp->encrypt_use_count++;
	rdp->encrypt_checksum_use_count++;
	rc = TRUE;
fail:
	LeaveCriticalSection(&rdp->critical);
	return rc;
}