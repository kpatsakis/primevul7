cert_hike(
	struct peer *peer,	/* peer structure pointer */
	struct cert_info *yp	/* issuer certificate */
	)
{
	struct cert_info *xp;	/* subject certificate */
	X509	*cert;		/* X509 certificate */
	const u_char *ptr;

	/*
	 * Save the issuer on the new certificate, but remember the old
	 * one.
	 */
	if (peer->issuer != NULL)
		free(peer->issuer);
	peer->issuer = estrdup(yp->issuer);
	xp = peer->xinfo;
	peer->xinfo = yp;

	/*
	 * If subject Y matches issuer Y, then the certificate trail is
	 * complete. If Y is not trusted, the server certificate has yet
	 * been signed, so keep trying. Otherwise, save the group key
	 * and light the valid bit. If the host certificate is trusted,
	 * do not execute a sign exchange. If no identity scheme is in
	 * use, light the identity and proventic bits.
	 */
	if (strcmp(yp->subject, yp->issuer) == 0) {
		if (!(yp->flags & CERT_TRUST))
			return (XEVNT_OK);

		/*
		 * If the server has an an identity scheme, fetch the
		 * identity credentials. If not, the identity is
		 * verified only by the trusted certificate. The next
		 * signature will set the server proventic.
		 */
		peer->crypto |= CRYPTO_FLAG_CERT;
		peer->grpkey = yp->grpkey;
		if (peer->ident == NULL || !(peer->crypto &
		    CRYPTO_FLAG_MASK))
			peer->crypto |= CRYPTO_FLAG_VRFY;
	}

	/*
	 * If X exists, verify signature X using public key Y.
	 */
	if (xp == NULL)
		return (XEVNT_OK);

	ptr = (u_char *)xp->cert.ptr;
	cert = d2i_X509(NULL, &ptr, ntohl(xp->cert.vallen));
	if (cert == NULL) {
		xp->flags |= CERT_ERROR;
		return (XEVNT_CRT);
	}
	if (X509_verify(cert, yp->pkey) <= 0) {
		X509_free(cert);
		xp->flags |= CERT_ERROR;
		return (XEVNT_VFY);
	}
	X509_free(cert);

	/*
	 * Signature X is valid only if it begins during the
	 * lifetime of Y. 
	 */
	if ((calcomp(&(xp->first), &(yp->first)) < 0)
	|| (calcomp(&(xp->first), &(yp->last)) > 0)) {
		xp->flags |= CERT_ERROR;
		return (XEVNT_PER);
	}
	xp->flags |= CERT_SIGN;
	return (XEVNT_OK);
}