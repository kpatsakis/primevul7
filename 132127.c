BOOL security_master_secret(const BYTE* premaster_secret, const BYTE* client_random,
                            const BYTE* server_random, BYTE* output)
{
	/* MasterSecret = PremasterHash('A') + PremasterHash('BB') + PremasterHash('CCC') */
	return security_premaster_hash("A", 1, premaster_secret, client_random, server_random,
	                               &output[0]) &&
	       security_premaster_hash("BB", 2, premaster_secret, client_random, server_random,
	                               &output[16]) &&
	       security_premaster_hash("CCC", 3, premaster_secret, client_random, server_random,
	                               &output[32]);
}