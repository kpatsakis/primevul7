BOOL security_licensing_encryption_key(const BYTE* session_key_blob, const BYTE* client_random,
                                       const BYTE* server_random, BYTE* output)
{
	/* LicensingEncryptionKey = MD5(Second128Bits(SessionKeyBlob) + ClientRandom + ServerRandom))
	 * Allow FIPS use of MD5 here, this is just used for creating the licensing encryption key as
	 * described in MS-RDPELE. This is for RDP licensing packets which will already be encrypted
	 * under FIPS, so the use of MD5 here is not for sensitive data protection. */
	return security_md5_16_32_32_Allow_FIPS(&session_key_blob[16], client_random, server_random,
	                                        output);
}