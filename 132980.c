crypto_bob(
	struct exten *ep,	/* extension pointer */
	struct value *vp	/* value pointer */
	)
{
	DSA	*dsa;		/* IFF parameters */
	DSA_SIG	*sdsa;		/* DSA signature context fake */
	BN_CTX	*bctx;		/* BIGNUM context */
	EVP_MD_CTX ctx;		/* signature context */
	tstamp_t tstamp;	/* NTP timestamp */
	BIGNUM	*bn, *bk, *r;
	u_char	*ptr;
	u_int	len;		/* extension field value length */

	/*
	 * If the IFF parameters are not valid, something awful
	 * happened or we are being tormented.
	 */
	if (iffkey_info == NULL) {
		msyslog(LOG_NOTICE, "crypto_bob: scheme unavailable");
		return (XEVNT_ID);
	}
	dsa = iffkey_info->pkey->pkey.dsa;

	/*
	 * Extract r from the challenge.
	 */
	len = exten_payload_size(ep);
	if (len == 0 || len > MAX_VALLEN)
		return (XEVNT_LEN);
	if ((r = BN_bin2bn((u_char *)ep->pkt, len, NULL)) == NULL) {
		msyslog(LOG_ERR, "crypto_bob: %s",
		    ERR_error_string(ERR_get_error(), NULL));
		return (XEVNT_ERR);
	}

	/*
	 * Bob rolls random k (0 < k < q), computes y = k + b r mod q
	 * and x = g^k mod p, then sends (y, hash(x)) to Alice.
	 */
	bctx = BN_CTX_new(); bk = BN_new(); bn = BN_new();
	sdsa = DSA_SIG_new();
	BN_rand(bk, len * 8, -1, 1);		/* k */
	BN_mod_mul(bn, dsa->priv_key, r, dsa->q, bctx); /* b r mod q */
	BN_add(bn, bn, bk);
	BN_mod(bn, bn, dsa->q, bctx);		/* k + b r mod q */
	sdsa->r = BN_dup(bn);
	BN_mod_exp(bk, dsa->g, bk, dsa->p, bctx); /* g^k mod p */
	bighash(bk, bk);
	sdsa->s = BN_dup(bk);
	BN_CTX_free(bctx);
	BN_free(r); BN_free(bn); BN_free(bk);
#ifdef DEBUG
	if (debug > 1)
		DSA_print_fp(stdout, dsa, 0);
#endif

	/*
	 * Encode the values in ASN.1 and sign. The filestamp is from
	 * the local file.
	 */
	len = i2d_DSA_SIG(sdsa, NULL);
	if (len == 0) {
		msyslog(LOG_ERR, "crypto_bob: %s",
		    ERR_error_string(ERR_get_error(), NULL));
		DSA_SIG_free(sdsa);
		return (XEVNT_ERR);
	}
	if (len > MAX_VALLEN) {
		msyslog(LOG_ERR, "crypto_bob: signature is too big: %u",
		    len);
		DSA_SIG_free(sdsa);
		return (XEVNT_LEN);
	}
	memset(vp, 0, sizeof(struct value));
	tstamp = crypto_time();
	vp->tstamp = htonl(tstamp);
	vp->fstamp = htonl(iffkey_info->fstamp);
	vp->vallen = htonl(len);
	ptr = emalloc(len);
	vp->ptr = ptr;
	i2d_DSA_SIG(sdsa, &ptr);
	DSA_SIG_free(sdsa);
	if (tstamp == 0)
		return (XEVNT_OK);

	/* XXX: more validation to make sure the sign fits... */
	vp->sig = emalloc(sign_siglen);
	EVP_SignInit(&ctx, sign_digest);
	EVP_SignUpdate(&ctx, (u_char *)&vp->tstamp, 12);
	EVP_SignUpdate(&ctx, vp->ptr, len);
	if (EVP_SignFinal(&ctx, vp->sig, &len, sign_pkey)) {
		INSIST(len <= sign_siglen);
		vp->siglen = htonl(len);
	}
	return (XEVNT_OK);
}