void license_encrypt_premaster_secret(rdpLicense* license)
{
	BYTE* EncryptedPremasterSecret;

	license_get_server_rsa_public_key(license);

#ifdef WITH_DEBUG_LICENSE
	fprintf(stderr, "Modulus (%d bits):\n", license->ModulusLength * 8);
	winpr_HexDump(license->Modulus, license->ModulusLength);
	fprintf(stderr, "\n");

	fprintf(stderr, "Exponent:\n");
	winpr_HexDump(license->Exponent, 4);
	fprintf(stderr, "\n");
#endif

	EncryptedPremasterSecret = (BYTE*) malloc(license->ModulusLength);
	ZeroMemory(EncryptedPremasterSecret, license->ModulusLength);
	license->EncryptedPremasterSecret->type = BB_RANDOM_BLOB;
	license->EncryptedPremasterSecret->length = PREMASTER_SECRET_LENGTH;

#ifndef LICENSE_NULL_PREMASTER_SECRET
	license->EncryptedPremasterSecret->length =
		crypto_rsa_public_encrypt(license->PremasterSecret, PREMASTER_SECRET_LENGTH,
			license->ModulusLength, license->Modulus, license->Exponent, EncryptedPremasterSecret);
#endif

	license->EncryptedPremasterSecret->data = EncryptedPremasterSecret;
}