cert_install(
	struct exten *ep,	/* cert info/value */
	struct peer *peer	/* peer structure */
	)
{
	struct cert_info *cp, *xp, **zp;

	/*
	 * Parse and validate the signed certificate. If valid,
	 * construct the info/value structure; otherwise, scamper home
	 * empty handed.
	 */
	if ((cp = cert_parse((u_char *)ep->pkt, (long)ntohl(ep->vallen),
	    (tstamp_t)ntohl(ep->fstamp))) == NULL)
		return (NULL);

	/*
	 * Scan certificate list looking for another certificate with
	 * the same subject and issuer. If another is found with the
	 * same or older filestamp, unlink it and return the goodies to
	 * the heap. If another is found with a later filestamp, discard
	 * the new one and leave the building with the old one.
	 *
	 * Make a note to study this issue again. An earlier certificate
	 * with a long lifetime might be overtaken by a later
	 * certificate with a short lifetime, thus invalidating the
	 * earlier signature. However, we gotta find a way to leak old
	 * stuff from the cache, so we do it anyway. 
	 */
	zp = &cinfo;
	for (xp = cinfo; xp != NULL; xp = xp->link) {
		if (strcmp(cp->subject, xp->subject) == 0 &&
		    strcmp(cp->issuer, xp->issuer) == 0) {
			if (ntohl(cp->cert.fstamp) <=
			    ntohl(xp->cert.fstamp)) {
				cert_free(cp);
				cp = xp;
			} else {
				*zp = xp->link;
				cert_free(xp);
				xp = NULL;
			}
			break;
		}
		zp = &xp->link;
	}
	if (xp == NULL) {
		cp->link = cinfo;
		cinfo = cp;
	}
	cp->flags |= CERT_VALID;
	crypto_update();
	return (cp);
}