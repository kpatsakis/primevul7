crypto_alice(
	struct peer *peer,	/* peer pointer */
	struct value *vp	/* value pointer */
	)
{
	DSA	*dsa;		/* IFF parameters */
	BN_CTX	*bctx;		/* BIGNUM context */
	EVP_MD_CTX ctx;		/* signature context */
	tstamp_t tstamp;
	u_int	len;

	/*
	 * The identity parameters must have correct format and content.
	 */
	if (peer->ident_pkey == NULL) {
		msyslog(LOG_NOTICE, "crypto_alice: scheme unavailable");
		return (XEVNT_ID);
	}

	if ((dsa = peer->ident_pkey->pkey->pkey.dsa) == NULL) {
		msyslog(LOG_NOTICE, "crypto_alice: defective key");
		return (XEVNT_PUB);
	}

	/*
	 * Roll new random r (0 < r < q).
	 */
	if (peer->iffval != NULL)
		BN_free(peer->iffval);
	peer->iffval = BN_new();
	len = BN_num_bytes(dsa->q);
	BN_rand(peer->iffval, len * 8, -1, 1);	/* r mod q*/
	bctx = BN_CTX_new();
	BN_mod(peer->iffval, peer->iffval, dsa->q, bctx);
	BN_CTX_free(bctx);

	/*
	 * Sign and send to Bob. The filestamp is from the local file.
	 */
	memset(vp, 0, sizeof(struct value));
	tstamp = crypto_time();
	vp->tstamp = htonl(tstamp);
	vp->fstamp = htonl(peer->ident_pkey->fstamp);
	vp->vallen = htonl(len);
	vp->ptr = emalloc(len);
	BN_bn2bin(peer->iffval, vp->ptr);
	if (tstamp == 0)
		return (XEVNT_OK);

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