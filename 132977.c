crypto_iff(
	struct exten *ep,	/* extension pointer */
	struct peer *peer	/* peer structure pointer */
	)
{
	DSA	*dsa;		/* IFF parameters */
	BN_CTX	*bctx;		/* BIGNUM context */
	DSA_SIG	*sdsa;		/* DSA parameters */
	BIGNUM	*bn, *bk;
	u_int	len;
	const u_char *ptr;
	int	temp;

	/*
	 * If the IFF parameters are not valid or no challenge was sent,
	 * something awful happened or we are being tormented.
	 */
	if (peer->ident_pkey == NULL) {
		msyslog(LOG_NOTICE, "crypto_iff: scheme unavailable");
		return (XEVNT_ID);
	}
	if (ntohl(ep->fstamp) != peer->ident_pkey->fstamp) {
		msyslog(LOG_NOTICE, "crypto_iff: invalid filestamp %u",
		    ntohl(ep->fstamp));
		return (XEVNT_FSP);
	}
	if ((dsa = peer->ident_pkey->pkey->pkey.dsa) == NULL) {
		msyslog(LOG_NOTICE, "crypto_iff: defective key");
		return (XEVNT_PUB);
	}
	if (peer->iffval == NULL) {
		msyslog(LOG_NOTICE, "crypto_iff: missing challenge");
		return (XEVNT_ID);
	}

	/*
	 * Extract the k + b r and g^k values from the response.
	 */
	bctx = BN_CTX_new(); bk = BN_new(); bn = BN_new();
	len = ntohl(ep->vallen);
	ptr = (u_char *)ep->pkt;
	if ((sdsa = d2i_DSA_SIG(NULL, &ptr, len)) == NULL) {
		BN_free(bn); BN_free(bk); BN_CTX_free(bctx);
		msyslog(LOG_ERR, "crypto_iff: %s",
		    ERR_error_string(ERR_get_error(), NULL));
		return (XEVNT_ERR);
	}

	/*
	 * Compute g^(k + b r) g^(q - b)r mod p.
	 */
	BN_mod_exp(bn, dsa->pub_key, peer->iffval, dsa->p, bctx);
	BN_mod_exp(bk, dsa->g, sdsa->r, dsa->p, bctx);
	BN_mod_mul(bn, bn, bk, dsa->p, bctx);

	/*
	 * Verify the hash of the result matches hash(x).
	 */
	bighash(bn, bn);
	temp = BN_cmp(bn, sdsa->s);
	BN_free(bn); BN_free(bk); BN_CTX_free(bctx);
	BN_free(peer->iffval);
	peer->iffval = NULL;
	DSA_SIG_free(sdsa);
	if (temp == 0)
		return (XEVNT_OK);

	msyslog(LOG_NOTICE, "crypto_iff: identity not verified");
	return (XEVNT_ID);
}