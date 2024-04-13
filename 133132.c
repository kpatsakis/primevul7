void license_write_new_license_request_packet(rdpLicense* license, wStream* s)
{
	UINT32 PlatformId;
	UINT32 PreferredKeyExchangeAlg = KEY_EXCHANGE_ALG_RSA;

	PlatformId = CLIENT_OS_ID_WINNT_POST_52 | CLIENT_IMAGE_ID_MICROSOFT;

	Stream_Write_UINT32(s, PreferredKeyExchangeAlg); /* PreferredKeyExchangeAlg (4 bytes) */
	Stream_Write_UINT32(s, PlatformId); /* PlatformId (4 bytes) */
	Stream_Write(s, license->ClientRandom, 32); /* ClientRandom (32 bytes) */
	license_write_encrypted_premaster_secret_blob(s, license->EncryptedPremasterSecret, license->ModulusLength); /* EncryptedPremasterSecret */
	license_write_binary_blob(s, license->ClientUserName); /* ClientUserName */
	license_write_binary_blob(s, license->ClientMachineName); /* ClientMachineName */

#ifdef WITH_DEBUG_LICENSE
	fprintf(stderr, "PreferredKeyExchangeAlg: 0x%08X\n", PreferredKeyExchangeAlg);
	fprintf(stderr, "\n");

	fprintf(stderr, "ClientRandom:\n");
	winpr_HexDump(license->ClientRandom, 32);
	fprintf(stderr, "\n");

	fprintf(stderr, "EncryptedPremasterSecret\n");
	winpr_HexDump(license->EncryptedPremasterSecret->data, license->EncryptedPremasterSecret->length);
	fprintf(stderr, "\n");

	fprintf(stderr, "ClientUserName (%d): %s\n", license->ClientUserName->length, (char*) license->ClientUserName->data);
	fprintf(stderr, "\n");

	fprintf(stderr, "ClientMachineName (%d): %s\n", license->ClientMachineName->length, (char*) license->ClientMachineName->data);
	fprintf(stderr, "\n");
#endif
}