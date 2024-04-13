static BOOL security_premaster_hash(const char* input, int length, const BYTE* premaster_secret,
                                    const BYTE* client_random, const BYTE* server_random,
                                    BYTE* output)
{
	/* PremasterHash(Input) = SaltedHash(PremasterSecret, Input, ClientRandom, ServerRandom) */
	return security_salted_hash(premaster_secret, (BYTE*)input, length, client_random,
	                            server_random, output);
}