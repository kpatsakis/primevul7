make_keylist(
	struct peer *peer,	/* peer structure pointer */
	struct interface *dstadr /* interface */
	)
{
	EVP_MD_CTX ctx;		/* signature context */
	tstamp_t tstamp;	/* NTP timestamp */
	struct autokey *ap;	/* autokey pointer */
	struct value *vp;	/* value pointer */
	keyid_t	keyid = 0;	/* next key ID */
	keyid_t	cookie;		/* private value */
	long	lifetime;
	u_int	len, mpoll;
	int	i;

	if (!dstadr)
		return XEVNT_ERR;
	
	/*
	 * Allocate the key list if necessary.
	 */
	tstamp = crypto_time();
	if (peer->keylist == NULL)
		peer->keylist = eallocarray(NTP_MAXSESSION,
					    sizeof(keyid_t));

	/*
	 * Generate an initial key ID which is unique and greater than
	 * NTP_MAXKEY.
	 */
	while (1) {
		keyid = ntp_random() & 0xffffffff;
		if (keyid <= NTP_MAXKEY)
			continue;

		if (authhavekey(keyid))
			continue;
		break;
	}

	/*
	 * Generate up to NTP_MAXSESSION session keys. Stop if the
	 * next one would not be unique or not a session key ID or if
	 * it would expire before the next poll. The private value
	 * included in the hash is zero if broadcast mode, the peer
	 * cookie if client mode or the host cookie if symmetric modes.
	 */
	mpoll = 1 << min(peer->ppoll, peer->hpoll);
	lifetime = min(1U << sys_automax, NTP_MAXSESSION * mpoll);
	if (peer->hmode == MODE_BROADCAST)
		cookie = 0;
	else
		cookie = peer->pcookie;
	for (i = 0; i < NTP_MAXSESSION; i++) {
		peer->keylist[i] = keyid;
		peer->keynumber = i;
		keyid = session_key(&dstadr->sin, &peer->srcadr, keyid,
		    cookie, lifetime + mpoll);
		lifetime -= mpoll;
		if (auth_havekey(keyid) || keyid <= NTP_MAXKEY ||
		    lifetime < 0 || tstamp == 0)
			break;
	}

	/*
	 * Save the last session key ID, sequence number and timestamp,
	 * then sign these values for later retrieval by the clients. Be
	 * careful not to use invalid key media. Use the public values
	 * timestamp as filestamp. 
	 */
	vp = &peer->sndval;
	if (vp->ptr == NULL)
		vp->ptr = emalloc(sizeof(struct autokey));
	ap = (struct autokey *)vp->ptr;
	ap->seq = htonl(peer->keynumber);
	ap->key = htonl(keyid);
	vp->tstamp = htonl(tstamp);
	vp->fstamp = hostval.tstamp;
	vp->vallen = htonl(sizeof(struct autokey));
	vp->siglen = 0;
	if (tstamp != 0) {
		if (vp->sig == NULL)
			vp->sig = emalloc(sign_siglen);
		EVP_SignInit(&ctx, sign_digest);
		EVP_SignUpdate(&ctx, (u_char *)vp, 12);
		EVP_SignUpdate(&ctx, vp->ptr, sizeof(struct autokey));
		if (EVP_SignFinal(&ctx, vp->sig, &len, sign_pkey)) {
			INSIST(len <= sign_siglen);
			vp->siglen = htonl(len);
			peer->flags |= FLAG_ASSOC;
		}
	}
	DPRINTF(1, ("make_keys: %d %08x %08x ts %u fs %u poll %d\n",
		    peer->keynumber, keyid, cookie, ntohl(vp->tstamp),
		    ntohl(vp->fstamp), peer->hpoll));
	return (XEVNT_OK);
}