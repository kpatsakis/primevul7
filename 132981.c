crypto_args(
	struct peer *peer,	/* peer structure pointer */
	u_int	opcode,		/* operation code */
	associd_t associd,	/* association ID */
	char	*str		/* argument string */
	)
{
	tstamp_t tstamp;	/* NTP timestamp */
	struct exten *ep;	/* extension field pointer */
	u_int	len;		/* extension field length */
	size_t	slen = 0;

	tstamp = crypto_time();
	len = sizeof(struct exten);
	if (str != NULL) {
		slen = strlen(str);
		INSIST(slen < MAX_VALLEN);
		len += slen;
	}
	ep = emalloc_zero(len);
	if (opcode == 0)
		return (ep);

	REQUIRE(0 == (len    & ~0x0000ffff));
	REQUIRE(0 == (opcode & ~0xffff0000));

	ep->opcode = htonl(opcode + len);
	ep->associd = htonl(associd);
	ep->tstamp = htonl(tstamp);
	ep->fstamp = hostval.tstamp;
	ep->vallen = 0;
	if (str != NULL) {
		ep->vallen = htonl(slen);
		memcpy((char *)ep->pkt, str, slen);
	}
	return (ep);
}