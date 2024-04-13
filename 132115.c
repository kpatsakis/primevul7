BOOL security_decrypt(BYTE* data, size_t length, rdpRdp* rdp)
{
	if (rdp->rc4_decrypt_key == NULL)
		return FALSE;

	if (rdp->decrypt_use_count >= 4096)
	{
		if (!security_key_update(rdp->decrypt_key, rdp->decrypt_update_key, rdp->rc4_key_len, rdp))
			return FALSE;

		winpr_RC4_Free(rdp->rc4_decrypt_key);
		rdp->rc4_decrypt_key = winpr_RC4_New(rdp->decrypt_key, rdp->rc4_key_len);

		if (!rdp->rc4_decrypt_key)
			return FALSE;

		rdp->decrypt_use_count = 0;
	}

	if (!winpr_RC4_Update(rdp->rc4_decrypt_key, length, data, data))
		return FALSE;

	rdp->decrypt_use_count += 1;
	rdp->decrypt_checksum_use_count++;
	return TRUE;
}