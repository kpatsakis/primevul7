BOOL security_fips_encrypt(BYTE* data, size_t length, rdpRdp* rdp)
{
	BOOL rc = FALSE;
	size_t olen;

	EnterCriticalSection(&rdp->critical);
	if (!winpr_Cipher_Update(rdp->fips_encrypt, data, length, data, &olen))
		goto fail;

	rdp->encrypt_use_count++;
	rc = TRUE;
fail:
	LeaveCriticalSection(&rdp->critical);
	return rc;
}