crypto_send(
	struct exten *ep,	/* extension field pointer */
	struct value *vp,	/* value pointer */
	int	start		/* buffer offset */
	)
{
	u_int	len, vallen, siglen, opcode;
	u_int	i, j;

	/*
	 * Calculate extension field length and check for buffer
	 * overflow. Leave room for the MAC.
	 */
	len = 16;				/* XXX Document! */
	vallen = ntohl(vp->vallen);
	INSIST(vallen <= MAX_VALLEN);
	len += ((vallen + 3) / 4 + 1) * 4; 
	siglen = ntohl(vp->siglen);
	len += ((siglen + 3) / 4 + 1) * 4; 
	if (start + len > sizeof(struct pkt) - MAX_MAC_LEN)
		return (0);

	/*
	 * Copy timestamps.
	 */
	ep->tstamp = vp->tstamp;
	ep->fstamp = vp->fstamp;
	ep->vallen = vp->vallen;

	/*
	 * Copy value. If the data field is empty or zero length,
	 * encode an empty value with length zero.
	 */
	i = 0;
	if (vallen > 0 && vp->ptr != NULL) {
		j = vallen / 4;
		if (j * 4 < vallen)
			ep->pkt[i + j++] = 0;
		memcpy(&ep->pkt[i], vp->ptr, vallen);
		i += j;
	}

	/*
	 * Copy signature. If the signature field is empty or zero
	 * length, encode an empty signature with length zero.
	 */
	ep->pkt[i++] = vp->siglen;
	if (siglen > 0 && vp->sig != NULL) {
		j = siglen / 4;
		if (j * 4 < siglen)
			ep->pkt[i + j++] = 0;
		memcpy(&ep->pkt[i], vp->sig, siglen);
		/* i += j; */	/* We don't use i after this */
	}
	opcode = ntohl(ep->opcode);
	ep->opcode = htonl((opcode & 0xffff0000) | len); 
	ENSURE(len <= MAX_VALLEN);
	return (len);
}