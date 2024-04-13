crypto_key(
	char	*cp,		/* file name */
	char	*passwd1,	/* password */
	sockaddr_u *addr 	/* IP address */
	)
{
	FILE	*str;		/* file handle */
	struct pkey_info *pkp;	/* generic key */
	EVP_PKEY *pkey = NULL;	/* public/private key */
	tstamp_t fstamp;
	char	filename[MAXFILENAME]; /* name of key file */
	char	linkname[MAXFILENAME]; /* filestamp buffer) */
	char	statstr[NTP_MAXSTRLEN]; /* statistics for filegen */
	char	*ptr;

	/*
	 * Search the key cache for matching key and name.
	 */
	for (pkp = pkinfo; pkp != NULL; pkp = pkp->link) {
		if (strcmp(cp, pkp->name) == 0)
			return (pkp);
	}  

	/*
	 * Open the key file. If the first character of the file name is
	 * not '/', prepend the keys directory string. If something goes
	 * wrong, abandon ship.
	 */
	if (*cp == '/')
		strlcpy(filename, cp, sizeof(filename));
	else
		snprintf(filename, sizeof(filename), "%s/%s", keysdir,
		    cp);
	str = fopen(filename, "r");
	if (str == NULL)
		return (NULL);

	/*
	 * Read the filestamp, which is contained in the first line.
	 */
	if ((ptr = fgets(linkname, sizeof(linkname), str)) == NULL) {
		msyslog(LOG_ERR, "crypto_key: empty file %s",
		    filename);
		fclose(str);
		return (NULL);
	}
	if ((ptr = strrchr(ptr, '.')) == NULL) {
		msyslog(LOG_ERR, "crypto_key: no filestamp %s",
		    filename);
		fclose(str);
		return (NULL);
	}
	if (sscanf(++ptr, "%u", &fstamp) != 1) {
		msyslog(LOG_ERR, "crypto_key: invalid filestamp %s",
		    filename);
		fclose(str);
		return (NULL);
	}

	/*
	 * Read and decrypt PEM-encoded private key. If it fails to
	 * decrypt, game over.
	 */
	pkey = PEM_read_PrivateKey(str, NULL, NULL, passwd1);
	fclose(str);
	if (pkey == NULL) {
		msyslog(LOG_ERR, "crypto_key: %s",
		    ERR_error_string(ERR_get_error(), NULL));
		exit (-1);
	}

	/*
	 * Make a new entry in the key cache.
	 */
	pkp = emalloc(sizeof(struct pkey_info));
	pkp->link = pkinfo;
	pkinfo = pkp;
	pkp->pkey = pkey;
	pkp->name = estrdup(cp);
	pkp->fstamp = fstamp;

	/*
	 * Leave tracks in the cryptostats.
	 */
	if ((ptr = strrchr(linkname, '\n')) != NULL)
		*ptr = '\0'; 
	snprintf(statstr, sizeof(statstr), "%s mod %d", &linkname[2],
	    EVP_PKEY_size(pkey) * 8);
	record_crypto_stats(addr, statstr);
	
	DPRINTF(1, ("crypto_key: %s\n", statstr));
#ifdef DEBUG
	if (debug > 1) {
		if (pkey->type == EVP_PKEY_DSA)
			DSA_print_fp(stdout, pkey->pkey.dsa, 0);
		else if (pkey->type == EVP_PKEY_RSA)
			RSA_print_fp(stdout, pkey->pkey.rsa, 0);
	}
#endif
	return (pkp);
}