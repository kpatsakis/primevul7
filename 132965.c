crypto_verify(
	struct exten *ep,	/* extension pointer */
	struct value *vp,	/* value pointer */
	struct peer *peer	/* peer structure pointer */
	)
{
	EVP_PKEY *pkey;		/* server public key */
	EVP_MD_CTX ctx;		/* signature context */
	tstamp_t tstamp, tstamp1 = 0; /* timestamp */
	tstamp_t fstamp, fstamp1 = 0; /* filestamp */
	u_int	vallen;		/* value length */
	u_int	siglen;		/* signature length */
	u_int	opcode, len;
	int	i;

	/*
	 * We are extremely parannoyed. We require valid opcode, length,
	 * association ID, timestamp, filestamp, public key, digest,
	 * signature length and signature, where relevant. Note that
	 * preliminary length checks are done in the main loop.
	 */
	len = ntohl(ep->opcode) & 0x0000ffff;
	opcode = ntohl(ep->opcode) & 0xffff0000;

	/*
	 * Check for valid value header, association ID and extension
	 * field length. Remember, it is not an error to receive an
	 * unsolicited response; however, the response ID must match
	 * the association ID.
	 */
	if (opcode & CRYPTO_ERROR)
		return (XEVNT_ERR);

 	if (len < VALUE_LEN)
		return (XEVNT_LEN);

	if (opcode == (CRYPTO_AUTO | CRYPTO_RESP) && (peer->pmode ==
	    MODE_BROADCAST || (peer->cast_flags & MDF_BCLNT))) {
		if (ntohl(ep->associd) != peer->assoc)
			return (XEVNT_ERR);
	} else {
		if (ntohl(ep->associd) != peer->associd)
			return (XEVNT_ERR);
	}

	/*
	 * We have a valid value header. Check for valid value and
	 * signature field lengths. The extension field length must be
	 * long enough to contain the value header, value and signature.
	 * Note both the value and signature field lengths are rounded
	 * up to the next word (4 octets).
	 */
	vallen = ntohl(ep->vallen);
	if (   vallen == 0
	    || vallen > MAX_VALLEN)
		return (XEVNT_LEN);

	i = (vallen + 3) / 4;
	siglen = ntohl(ep->pkt[i++]);
	if (   siglen > MAX_VALLEN
	    || len - VALUE_LEN < ((vallen + 3) / 4) * 4
	    || len - VALUE_LEN - ((vallen + 3) / 4) * 4
	      < ((siglen + 3) / 4) * 4)
		return (XEVNT_LEN);

	/*
	 * Check for valid timestamp and filestamp. If the timestamp is
	 * zero, the sender is not synchronized and signatures are
	 * not possible. If nonzero the timestamp must not precede the
	 * filestamp. The timestamp and filestamp must not precede the
	 * corresponding values in the value structure, if present.
 	 */
	tstamp = ntohl(ep->tstamp);
	fstamp = ntohl(ep->fstamp);
	if (tstamp == 0)
		return (XEVNT_TSP);

	if (tstamp < fstamp)
		return (XEVNT_TSP);

	if (vp != NULL) {
		tstamp1 = ntohl(vp->tstamp);
		fstamp1 = ntohl(vp->fstamp);
		if (tstamp1 != 0 && fstamp1 != 0) {
			if (tstamp < tstamp1)
				return (XEVNT_TSP);

			if ((tstamp < fstamp1 || fstamp < fstamp1))
				return (XEVNT_FSP);
		}
	}

	/*
	 * At the time the certificate message is validated, the public
	 * key in the message is not available. Thus, don't try to
	 * verify the signature.
	 */
	if (opcode == (CRYPTO_CERT | CRYPTO_RESP))
		return (XEVNT_OK);

	/*
	 * Check for valid signature length, public key and digest
	 * algorithm.
	 */
	if (crypto_flags & peer->crypto & CRYPTO_FLAG_PRIV)
		pkey = sign_pkey;
	else
		pkey = peer->pkey;
	if (siglen == 0 || pkey == NULL || peer->digest == NULL)
		return (XEVNT_ERR);

	if (siglen != (u_int)EVP_PKEY_size(pkey))
		return (XEVNT_SGL);

	/*
	 * Darn, I thought we would never get here. Verify the
	 * signature. If the identity exchange is verified, light the
	 * proventic bit. What a relief.
	 */
	EVP_VerifyInit(&ctx, peer->digest);
	/* XXX: the "+ 12" needs to be at least documented... */
	EVP_VerifyUpdate(&ctx, (u_char *)&ep->tstamp, vallen + 12);
	if (EVP_VerifyFinal(&ctx, (u_char *)&ep->pkt[i], siglen,
	    pkey) <= 0)
		return (XEVNT_SIG);

	if (peer->crypto & CRYPTO_FLAG_VRFY)
		peer->crypto |= CRYPTO_FLAG_PROV;
	return (XEVNT_OK);
}