BOOL security_session_key_blob(const BYTE* master_secret, const BYTE* client_random,
                               const BYTE* server_random, BYTE* output)
{
	/* MasterHash = MasterHash('A') + MasterHash('BB') + MasterHash('CCC') */
	return security_master_hash("A", 1, master_secret, client_random, server_random, &output[0]) &&
	       security_master_hash("BB", 2, master_secret, client_random, server_random,
	                            &output[16]) &&
	       security_master_hash("CCC", 3, master_secret, client_random, server_random, &output[32]);
}