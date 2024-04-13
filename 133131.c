void license_decrypt_platform_challenge(rdpLicense* license)
{
	CryptoRc4 rc4;

	license->PlatformChallenge->data = (BYTE*) malloc(license->EncryptedPlatformChallenge->length);
	license->PlatformChallenge->length = license->EncryptedPlatformChallenge->length;

	rc4 = crypto_rc4_init(license->LicensingEncryptionKey, LICENSING_ENCRYPTION_KEY_LENGTH);

	crypto_rc4(rc4, license->EncryptedPlatformChallenge->length,
			license->EncryptedPlatformChallenge->data,
			license->PlatformChallenge->data);

	crypto_rc4_free(rc4);
}