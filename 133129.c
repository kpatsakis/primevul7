BOOL license_read_platform_challenge_packet(rdpLicense* license, wStream* s)
{
	BYTE MacData[16];
	UINT32 ConnectFlags = 0;

	DEBUG_LICENSE("Receiving Platform Challenge Packet");

	if (Stream_GetRemainingLength(s) < 4)
		return FALSE;

	Stream_Read_UINT32(s, ConnectFlags); /* ConnectFlags, Reserved (4 bytes) */

	/* EncryptedPlatformChallenge */
	license->EncryptedPlatformChallenge->type = BB_ANY_BLOB;
	license_read_binary_blob(s, license->EncryptedPlatformChallenge);
	license->EncryptedPlatformChallenge->type = BB_ENCRYPTED_DATA_BLOB;

	if (Stream_GetRemainingLength(s) < 16)
		return FALSE;

	Stream_Read(s, MacData, 16); /* MACData (16 bytes) */

	license_decrypt_platform_challenge(license);

#ifdef WITH_DEBUG_LICENSE
	fprintf(stderr, "ConnectFlags: 0x%08X\n", ConnectFlags);
	fprintf(stderr, "\n");

	fprintf(stderr, "EncryptedPlatformChallenge:\n");
	winpr_HexDump(license->EncryptedPlatformChallenge->data, license->EncryptedPlatformChallenge->length);
	fprintf(stderr, "\n");

	fprintf(stderr, "PlatformChallenge:\n");
	winpr_HexDump(license->PlatformChallenge->data, license->PlatformChallenge->length);
	fprintf(stderr, "\n");

	fprintf(stderr, "MacData:\n");
	winpr_HexDump(MacData, 16);
	fprintf(stderr, "\n");
#endif

	return TRUE;
}