crypto_ident(
	struct peer *peer	/* peer structure pointer */
	)
{
	char		filename[MAXFILENAME];
	const char *	scheme_name;
	u_int		scheme_id;

	/*
	 * We come here after the group trusted host has been found; its
	 * name defines the group name. Search the key cache for all
	 * keys matching the same group name in order IFF, GQ and MV.
	 * Use the first one available.
	 */
	scheme_name = NULL;
	if (peer->crypto & CRYPTO_FLAG_IFF) {
		scheme_name = "iff";
		scheme_id = CRYPTO_IFF;
	} else if (peer->crypto & CRYPTO_FLAG_GQ) {
		scheme_name = "gq";
		scheme_id = CRYPTO_GQ;
	} else if (peer->crypto & CRYPTO_FLAG_MV) {
		scheme_name = "mv";
		scheme_id = CRYPTO_MV;
	}

	if (scheme_name != NULL) {
		snprintf(filename, sizeof(filename), "ntpkey_%spar_%s",
		    scheme_name, peer->ident);
		peer->ident_pkey = crypto_key(filename, NULL,
		    &peer->srcadr);
		if (peer->ident_pkey != NULL)
			return scheme_id;
	}

	msyslog(LOG_NOTICE,
	    "crypto_ident: no identity parameters found for group %s",
	    peer->ident);

	return CRYPTO_NULL;
}