crypto_config(
	int	item,		/* configuration item */
	char	*cp		/* item name */
	)
{
	int	nid;

	DPRINTF(1, ("crypto_config: item %d %s\n", item, cp));

	switch (item) {

	/*
	 * Set host name (host).
	 */
	case CRYPTO_CONF_PRIV:
		if (NULL != host_filename)
			free(host_filename);
		host_filename = estrdup(cp);
		break;

	/*
	 * Set group name (ident).
	 */
	case CRYPTO_CONF_IDENT:
		if (NULL != ident_filename)
			free(ident_filename);
		ident_filename = estrdup(cp);
		break;

	/*
	 * Set private key password (pw).
	 */
	case CRYPTO_CONF_PW:
		if (NULL != passwd)
			free(passwd);
		passwd = estrdup(cp);
		break;

	/*
	 * Set random seed file name (randfile).
	 */
	case CRYPTO_CONF_RAND:
		if (NULL != rand_file)
			free(rand_file);
		rand_file = estrdup(cp);
		break;

	/*
	 * Set message digest NID.
	 */
	case CRYPTO_CONF_NID:
		nid = OBJ_sn2nid(cp);
		if (nid == 0)
			msyslog(LOG_ERR,
			    "crypto_config: invalid digest name %s", cp);
		else
			crypto_nid = nid;
		break;
	}
}