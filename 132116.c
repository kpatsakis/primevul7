BOOL security_establish_keys(const BYTE* client_random, rdpRdp* rdp)
{
	BYTE pre_master_secret[48];
	BYTE master_secret[48];
	BYTE session_key_blob[48];
	BYTE* server_random;
	BYTE salt[] = { 0xD1, 0x26, 0x9E }; /* 40 bits: 3 bytes, 56 bits: 1 byte */
	rdpSettings* settings;
	BOOL status;
	settings = rdp->settings;
	server_random = settings->ServerRandom;

	if (settings->EncryptionMethods == ENCRYPTION_METHOD_FIPS)
	{
		WINPR_DIGEST_CTX* sha1;
		BYTE client_encrypt_key_t[WINPR_SHA1_DIGEST_LENGTH + 1];
		BYTE client_decrypt_key_t[WINPR_SHA1_DIGEST_LENGTH + 1];

		if (!(sha1 = winpr_Digest_New()))
			return FALSE;

		if (!winpr_Digest_Init(sha1, WINPR_MD_SHA1) ||
		    !winpr_Digest_Update(sha1, client_random + 16, 16) ||
		    !winpr_Digest_Update(sha1, server_random + 16, 16) ||
		    !winpr_Digest_Final(sha1, client_encrypt_key_t, sizeof(client_encrypt_key_t)))
		{
			winpr_Digest_Free(sha1);
			return FALSE;
		}

		client_encrypt_key_t[20] = client_encrypt_key_t[0];

		if (!winpr_Digest_Init(sha1, WINPR_MD_SHA1) ||
		    !winpr_Digest_Update(sha1, client_random, 16) ||
		    !winpr_Digest_Update(sha1, server_random, 16) ||
		    !winpr_Digest_Final(sha1, client_decrypt_key_t, sizeof(client_decrypt_key_t)))
		{
			winpr_Digest_Free(sha1);
			return FALSE;
		}

		client_decrypt_key_t[20] = client_decrypt_key_t[0];

		if (!winpr_Digest_Init(sha1, WINPR_MD_SHA1) ||
		    !winpr_Digest_Update(sha1, client_decrypt_key_t, WINPR_SHA1_DIGEST_LENGTH) ||
		    !winpr_Digest_Update(sha1, client_encrypt_key_t, WINPR_SHA1_DIGEST_LENGTH) ||
		    !winpr_Digest_Final(sha1, rdp->fips_sign_key, WINPR_SHA1_DIGEST_LENGTH))
		{
			winpr_Digest_Free(sha1);
			return FALSE;
		}

		winpr_Digest_Free(sha1);

		if (rdp->settings->ServerMode)
		{
			fips_expand_key_bits(client_encrypt_key_t, rdp->fips_decrypt_key);
			fips_expand_key_bits(client_decrypt_key_t, rdp->fips_encrypt_key);
		}
		else
		{
			fips_expand_key_bits(client_encrypt_key_t, rdp->fips_encrypt_key);
			fips_expand_key_bits(client_decrypt_key_t, rdp->fips_decrypt_key);
		}
	}

	memcpy(pre_master_secret, client_random, 24);
	memcpy(pre_master_secret + 24, server_random, 24);

	if (!security_A(pre_master_secret, client_random, server_random, master_secret) ||
	    !security_X(master_secret, client_random, server_random, session_key_blob))
	{
		return FALSE;
	}

	memcpy(rdp->sign_key, session_key_blob, 16);

	if (rdp->settings->ServerMode)
	{
		status = security_md5_16_32_32(&session_key_blob[16], client_random, server_random,
		                               rdp->encrypt_key);
		status &= security_md5_16_32_32(&session_key_blob[32], client_random, server_random,
		                                rdp->decrypt_key);
	}
	else
	{
		/* Allow FIPS use of MD5 here, this is just used for generation of the SessionKeyBlob as
		 * described in MS-RDPELE. */
		/* This is for RDP licensing packets which will already be encrypted under FIPS, so the use
		 * of MD5 here is not */
		/* for sensitive data protection. */
		status = security_md5_16_32_32_Allow_FIPS(&session_key_blob[16], client_random,
		                                          server_random, rdp->decrypt_key);
		status &= security_md5_16_32_32_Allow_FIPS(&session_key_blob[32], client_random,
		                                           server_random, rdp->encrypt_key);
	}

	if (!status)
		return FALSE;

	if (settings->EncryptionMethods == ENCRYPTION_METHOD_40BIT)
	{
		memcpy(rdp->sign_key, salt, 3);
		memcpy(rdp->decrypt_key, salt, 3);
		memcpy(rdp->encrypt_key, salt, 3);
		rdp->rc4_key_len = 8;
	}
	else if (settings->EncryptionMethods == ENCRYPTION_METHOD_56BIT)
	{
		memcpy(rdp->sign_key, salt, 1);
		memcpy(rdp->decrypt_key, salt, 1);
		memcpy(rdp->encrypt_key, salt, 1);
		rdp->rc4_key_len = 8;
	}
	else if (settings->EncryptionMethods == ENCRYPTION_METHOD_128BIT)
	{
		rdp->rc4_key_len = 16;
	}

	memcpy(rdp->decrypt_update_key, rdp->decrypt_key, 16);
	memcpy(rdp->encrypt_update_key, rdp->encrypt_key, 16);
	rdp->decrypt_use_count = 0;
	rdp->decrypt_checksum_use_count = 0;
	rdp->encrypt_use_count = 0;
	rdp->encrypt_checksum_use_count = 0;
	return TRUE;
}