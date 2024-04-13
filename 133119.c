void license_send_platform_challenge_response_packet(rdpLicense* license)
{
	wStream* s;
	int length;
	BYTE* buffer;
	CryptoRc4 rc4;
	BYTE mac_data[16];

	DEBUG_LICENSE("Sending Platform Challenge Response Packet");

	s = license_send_stream_init(license);

	license->EncryptedPlatformChallenge->type = BB_DATA_BLOB;
	length = license->PlatformChallenge->length + HWID_LENGTH;

	buffer = (BYTE*) malloc(length);
	CopyMemory(buffer, license->PlatformChallenge->data, license->PlatformChallenge->length);
	CopyMemory(&buffer[license->PlatformChallenge->length], license->HardwareId, HWID_LENGTH);
	security_mac_data(license->MacSaltKey, buffer, length, mac_data);
	free(buffer);

	buffer = (BYTE*) malloc(HWID_LENGTH);
	rc4 = crypto_rc4_init(license->LicensingEncryptionKey, LICENSING_ENCRYPTION_KEY_LENGTH);
	crypto_rc4(rc4, HWID_LENGTH, license->HardwareId, buffer);
	crypto_rc4_free(rc4);

	license->EncryptedHardwareId->type = BB_DATA_BLOB;
	license->EncryptedHardwareId->data = buffer;
	license->EncryptedHardwareId->length = HWID_LENGTH;

#ifdef WITH_DEBUG_LICENSE
	fprintf(stderr, "LicensingEncryptionKey:\n");
	winpr_HexDump(license->LicensingEncryptionKey, 16);
	fprintf(stderr, "\n");

	fprintf(stderr, "HardwareId:\n");
	winpr_HexDump(license->HardwareId, HWID_LENGTH);
	fprintf(stderr, "\n");

	fprintf(stderr, "EncryptedHardwareId:\n");
	winpr_HexDump(license->EncryptedHardwareId->data, HWID_LENGTH);
	fprintf(stderr, "\n");
#endif

	license_write_platform_challenge_response_packet(license, s, mac_data);

	license_send(license, s, PLATFORM_CHALLENGE_RESPONSE);
}