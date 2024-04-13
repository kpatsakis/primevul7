static BOOL security_master_hash(const char* input, int length, const BYTE* master_secret,
                                 const BYTE* client_random, const BYTE* server_random, BYTE* output)
{
	/* MasterHash(Input) = SaltedHash(MasterSecret, Input, ServerRandom, ClientRandom) */
	return security_salted_hash(master_secret, (const BYTE*)input, length, server_random,
	                            client_random, output);
}