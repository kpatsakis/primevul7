void license_generate_hwid(rdpLicense* license)
{
	CryptoMd5 md5;
	BYTE* mac_address;

	ZeroMemory(license->HardwareId, HWID_LENGTH);
	mac_address = license->rdp->transport->TcpIn->mac_address;

	md5 = crypto_md5_init();
	crypto_md5_update(md5, mac_address, 6);
	crypto_md5_final(md5, &license->HardwareId[HWID_PLATFORM_ID_LENGTH]);
}