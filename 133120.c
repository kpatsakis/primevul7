rdpLicense* license_new(rdpRdp* rdp)
{
	rdpLicense* license;

	license = (rdpLicense*) malloc(sizeof(rdpLicense));

	if (license != NULL)
	{
		ZeroMemory(license, sizeof(rdpLicense));

		license->rdp = rdp;
		license->state = LICENSE_STATE_AWAIT;
		license->certificate = certificate_new();
		license->ProductInfo = license_new_product_info();
		license->ErrorInfo = license_new_binary_blob(BB_ERROR_BLOB);
		license->KeyExchangeList = license_new_binary_blob(BB_KEY_EXCHG_ALG_BLOB);
		license->ServerCertificate = license_new_binary_blob(BB_CERTIFICATE_BLOB);
		license->ClientUserName = license_new_binary_blob(BB_CLIENT_USER_NAME_BLOB);
		license->ClientMachineName = license_new_binary_blob(BB_CLIENT_MACHINE_NAME_BLOB);
		license->PlatformChallenge = license_new_binary_blob(BB_ANY_BLOB);
		license->EncryptedPlatformChallenge = license_new_binary_blob(BB_ANY_BLOB);
		license->EncryptedPremasterSecret = license_new_binary_blob(BB_ANY_BLOB);
		license->EncryptedHardwareId = license_new_binary_blob(BB_ENCRYPTED_DATA_BLOB);
		license->ScopeList = license_new_scope_list();
		license_generate_randoms(license);
	}

	return license;
}