void license_generate_randoms(rdpLicense* license)
{
	ZeroMemory(license->ClientRandom, CLIENT_RANDOM_LENGTH); /* ClientRandom */
	ZeroMemory(license->PremasterSecret, PREMASTER_SECRET_LENGTH); /* PremasterSecret */

#ifndef LICENSE_NULL_CLIENT_RANDOM
	crypto_nonce(license->ClientRandom, CLIENT_RANDOM_LENGTH); /* ClientRandom */
#endif

#ifndef LICENSE_NULL_PREMASTER_SECRET
	crypto_nonce(license->PremasterSecret, PREMASTER_SECRET_LENGTH); /* PremasterSecret */
#endif
}