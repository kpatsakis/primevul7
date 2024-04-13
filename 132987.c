crypto_update(void)
{
	EVP_MD_CTX ctx;		/* message digest context */
	struct cert_info *cp;	/* certificate info/value */
	char	statstr[NTP_MAXSTRLEN]; /* statistics for filegen */
	u_int32	*ptr;
	u_int	len;
	leap_result_t leap_data;

	hostval.tstamp = htonl(crypto_time());
	if (hostval.tstamp == 0)
		return;

	/*
	 * Sign public key and timestamps. The filestamp is derived from
	 * the host key file extension from wherever the file was
	 * generated. 
	 */
	if (pubkey.vallen != 0) {
		pubkey.tstamp = hostval.tstamp;
		pubkey.siglen = 0;
		if (pubkey.sig == NULL)
			pubkey.sig = emalloc(sign_siglen);
		EVP_SignInit(&ctx, sign_digest);
		EVP_SignUpdate(&ctx, (u_char *)&pubkey, 12);
		EVP_SignUpdate(&ctx, pubkey.ptr, ntohl(pubkey.vallen));
		if (EVP_SignFinal(&ctx, pubkey.sig, &len, sign_pkey)) {
			INSIST(len <= sign_siglen);
			pubkey.siglen = htonl(len);
		}
	}

	/*
	 * Sign certificates and timestamps. The filestamp is derived
	 * from the certificate file extension from wherever the file
	 * was generated. Note we do not throw expired certificates
	 * away; they may have signed younger ones.
	 */
	for (cp = cinfo; cp != NULL; cp = cp->link) {
		cp->cert.tstamp = hostval.tstamp;
		cp->cert.siglen = 0;
		if (cp->cert.sig == NULL)
			cp->cert.sig = emalloc(sign_siglen);
		EVP_SignInit(&ctx, sign_digest);
		EVP_SignUpdate(&ctx, (u_char *)&cp->cert, 12);
		EVP_SignUpdate(&ctx, cp->cert.ptr,
		    ntohl(cp->cert.vallen));
		if (EVP_SignFinal(&ctx, cp->cert.sig, &len, sign_pkey)) {
			INSIST(len <= sign_siglen);
			cp->cert.siglen = htonl(len);
		}
	}

	/*
	 * Sign leapseconds values and timestamps. Note it is not an
	 * error to return null values.
	 */
	tai_leap.tstamp = hostval.tstamp;
	tai_leap.fstamp = hostval.fstamp;

	/* Get the leap second era. We might need a full lookup early
	 * after start, when the cache is not yet loaded.
	 */
	leapsec_frame(&leap_data);
	if ( ! memcmp(&leap_data.ebase, &leap_data.ttime, sizeof(vint64))) {
		time_t   now    = time(NULL);
		uint32_t nowntp = (uint32_t)now + JAN_1970;
		leapsec_query(&leap_data, nowntp, &now);
	}

	/* Create the data block. The protocol does not work without. */
	len = 3 * sizeof(u_int32);
	if (tai_leap.ptr == NULL || ntohl(tai_leap.vallen) != len) {
		free(tai_leap.ptr);
		tai_leap.ptr = emalloc(len);
		tai_leap.vallen = htonl(len);
	}
	ptr = (u_int32 *)tai_leap.ptr;
	if (leap_data.tai_offs > 10) {
		/* create a TAI / leap era block. The end time is a
		 * fake -- maybe we can do better.
		 */
		ptr[0] = htonl(leap_data.tai_offs);
		ptr[1] = htonl(leap_data.ebase.d_s.lo);
		if (leap_data.ttime.d_s.hi >= 0)
			ptr[2] = htonl(leap_data.ttime.D_s.lo +  7*86400);
		else
			ptr[2] = htonl(leap_data.ebase.D_s.lo + 25*86400);
	} else {
		/* no leap era available */
		memset(ptr, 0, len);
	}
	if (tai_leap.sig == NULL)
		tai_leap.sig = emalloc(sign_siglen);
	EVP_SignInit(&ctx, sign_digest);
	EVP_SignUpdate(&ctx, (u_char *)&tai_leap, 12);
	EVP_SignUpdate(&ctx, tai_leap.ptr, len);
	if (EVP_SignFinal(&ctx, tai_leap.sig, &len, sign_pkey)) {
		INSIST(len <= sign_siglen);
		tai_leap.siglen = htonl(len);
	}
	crypto_flags |= CRYPTO_FLAG_TAI;

	snprintf(statstr, sizeof(statstr), "signature update ts %u",
	    ntohl(hostval.tstamp)); 
	record_crypto_stats(NULL, statstr);
	DPRINTF(1, ("crypto_update: %s\n", statstr));
}