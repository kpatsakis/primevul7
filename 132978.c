crypto_mv(
	struct exten *ep,	/* extension pointer */
	struct peer *peer	/* peer structure pointer */
	)
{
	DSA	*dsa;		/* MV parameters */
	DSA	*sdsa;		/* DSA parameters */
	BN_CTX	*bctx;		/* BIGNUM context */
	BIGNUM	*k, *u, *v;
	u_int	len;
	const u_char *ptr;
	int	temp;

	/*
	 * If the MV parameters are not valid or no challenge was sent,
	 * something awful happened or we are being tormented.
	 */
	if (peer->ident_pkey == NULL) {
		msyslog(LOG_NOTICE, "crypto_mv: scheme unavailable");
		return (XEVNT_ID);
	}
	if (ntohl(ep->fstamp) != peer->ident_pkey->fstamp) {
		msyslog(LOG_NOTICE, "crypto_mv: invalid filestamp %u",
		    ntohl(ep->fstamp));
		return (XEVNT_FSP);
	}
	if ((dsa = peer->ident_pkey->pkey->pkey.dsa) == NULL) {
		msyslog(LOG_NOTICE, "crypto_mv: defective key");
		return (XEVNT_PUB);
	}
	if (peer->iffval == NULL) {
		msyslog(LOG_NOTICE, "crypto_mv: missing challenge");
		return (XEVNT_ID);
	}

	/*
	 * Extract the y, gbar and ghat values from the response.
	 */
	bctx = BN_CTX_new(); k = BN_new(); u = BN_new(); v = BN_new();
	len = ntohl(ep->vallen);
	ptr = (u_char *)ep->pkt;
	if ((sdsa = d2i_DSAparams(NULL, &ptr, len)) == NULL) {
		msyslog(LOG_ERR, "crypto_mv: %s",
		    ERR_error_string(ERR_get_error(), NULL));
		return (XEVNT_ERR);
	}

	/*
	 * Compute (gbar^xhat ghat^xbar) mod p.
	 */
	BN_mod_exp(u, sdsa->q, dsa->pub_key, dsa->p, bctx);
	BN_mod_exp(v, sdsa->g, dsa->priv_key, dsa->p, bctx);
	BN_mod_mul(u, u, v, dsa->p, bctx);
	BN_mod_mul(u, u, sdsa->p, dsa->p, bctx);

	/*
	 * The result should match r.
	 */
	temp = BN_cmp(u, peer->iffval);
	BN_CTX_free(bctx); BN_free(k); BN_free(u); BN_free(v);
	BN_free(peer->iffval);
	peer->iffval = NULL;
	DSA_free(sdsa);
	if (temp == 0)
		return (XEVNT_OK);

	msyslog(LOG_NOTICE, "crypto_mv: identity not verified");
	return (XEVNT_ID);
}