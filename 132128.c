static BOOL security_X(BYTE* master_secret, const BYTE* client_random, BYTE* server_random,
                       BYTE* output)
{
	return security_premaster_hash("X", 1, master_secret, client_random, server_random,
	                               &output[0]) &&
	       security_premaster_hash("YY", 2, master_secret, client_random, server_random,
	                               &output[16]) &&
	       security_premaster_hash("ZZZ", 3, master_secret, client_random, server_random,
	                               &output[32]);
}