crypto_gq(
	struct exten *ep,	/* extension pointer */
	struct peer *peer	/* peer structure pointer */
	)
{
	RSA	*rsa;		/* GQ parameters */
	BN_CTX	*bctx;		/* BIGNUM context */
	DSA_SIG	*sdsa;		/* RSA signature context fake */
	BIGNUM	*y, *v;
	const u_char *ptr;
	long	len;
	u_int	temp;

	/*
	 * If the GQ parameters are not valid or no challenge was sent,
	 * something awful happened or we are being tormented. Note that
	 * the filestamp on the local key file can be greater than on
	 * the remote parameter file if the keys have been refreshed.
	 */
	if (peer->ident_pkey == NULL) {
		msyslog(LOG_NOTICE, "crypto_gq: scheme unavailable");
		return (XEVNT_ID);
	}
	if (ntohl(ep->fstamp) < peer->ident_pkey->fstamp) {
		msyslog(LOG_NOTICE, "crypto_gq: invalid filestamp %u",
		    ntohl(ep->fstamp));
		return (XEVNT_FSP);
	}
	if ((rsa = peer->ident_pkey->pkey->pkey.rsa) == NULL) {
		msyslog(LOG_NOTICE, "crypto_gq: defective key");
		return (XEVNT_PUB);
	}
	if (peer->iffval == NULL) {
		msyslog(LOG_NOTICE, "crypto_gq: missing challenge");
		return (XEVNT_ID);
	}

	/*
	 * Extract the y = k u^r and hash(x = k^b) values from the
	 * response.
	 */
	bctx = BN_CTX_new(); y = BN_new(); v = BN_new();
	len = ntohl(ep->vallen);
	ptr = (u_char *)ep->pkt;
	if ((sdsa = d2i_DSA_SIG(NULL, &ptr, len)) == NULL) {
		BN_CTX_free(bctx); BN_free(y); BN_free(v);
		msyslog(LOG_ERR, "crypto_gq: %s",
		    ERR_error_string(ERR_get_error(), NULL));
		return (XEVNT_ERR);
	}

	/*
	 * Compute v^r y^b mod n.
	 */
	if (peer->grpkey == NULL) {
		msyslog(LOG_NOTICE, "crypto_gq: missing group key");
		return (XEVNT_ID);
	}
	BN_mod_exp(v, peer->grpkey, peer->iffval, rsa->n, bctx);
						/* v^r mod n */
	BN_mod_exp(y, sdsa->r, rsa->e, rsa->n, bctx); /* y^b mod n */
	BN_mod_mul(y, v, y, rsa->n, bctx);	/* v^r y^b mod n */

	/*
	 * Verify the hash of the result matches hash(x).
	 */
	bighash(y, y);
	temp = BN_cmp(y, sdsa->s);
	BN_CTX_free(bctx); BN_free(y); BN_free(v);
	BN_free(peer->iffval);
	peer->iffval = NULL;
	DSA_SIG_free(sdsa);
	if (temp == 0)
		return (XEVNT_OK);

	msyslog(LOG_NOTICE, "crypto_gq: identity not verified");
	return (XEVNT_ID);
}