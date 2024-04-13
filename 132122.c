void security_mac_salt_key(const BYTE* session_key_blob, const BYTE* client_random,
                           const BYTE* server_random, BYTE* output)
{
	/* MacSaltKey = First128Bits(SessionKeyBlob) */
	memcpy(output, session_key_blob, 16);
}