crypto_cert(
	char	*cp		/* file name */
	)
{
	struct cert_info *ret; /* certificate information */
	FILE	*str;		/* file handle */
	char	filename[MAXFILENAME]; /* name of certificate file */
	char	linkname[MAXFILENAME]; /* filestamp buffer */
	char	statstr[NTP_MAXSTRLEN]; /* statistics for filegen */
	tstamp_t fstamp;	/* filestamp */
	long	len;
	char	*ptr;
	char	*name, *header;
	u_char	*data;

	/*
	 * Open the certificate file. If the first character of the file
	 * name is not '/', prepend the keys directory string. If
	 * something goes wrong, abandon ship.
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
		msyslog(LOG_ERR, "crypto_cert: empty file %s",
		    filename);
		fclose(str);
		return (NULL);
	}
	if ((ptr = strrchr(ptr, '.')) == NULL) {
		msyslog(LOG_ERR, "crypto_cert: no filestamp %s",
		    filename);
		fclose(str);
		return (NULL);
	}
	if (sscanf(++ptr, "%u", &fstamp) != 1) {
		msyslog(LOG_ERR, "crypto_cert: invalid filestamp %s",
		    filename);
		fclose(str);
		return (NULL);
	}

	/*
	 * Read PEM-encoded certificate and install.
	 */
	if (!PEM_read(str, &name, &header, &data, &len)) {
		msyslog(LOG_ERR, "crypto_cert: %s",
		    ERR_error_string(ERR_get_error(), NULL));
		fclose(str);
		return (NULL);
	}
	fclose(str);
	free(header);
	if (strcmp(name, "CERTIFICATE") != 0) {
		msyslog(LOG_NOTICE, "crypto_cert: wrong PEM type %s",
		    name);
		free(name);
		free(data);
		return (NULL);
	}
	free(name);

	/*
	 * Parse certificate and generate info/value structure. The
	 * pointer and copy nonsense is due something broken in Solaris.
	 */
	ret = cert_parse(data, len, fstamp);
	free(data);
	if (ret == NULL)
		return (NULL);

	if ((ptr = strrchr(linkname, '\n')) != NULL)
		*ptr = '\0'; 
	snprintf(statstr, sizeof(statstr), "%s 0x%x len %lu",
	    &linkname[2], ret->flags, len);
	record_crypto_stats(NULL, statstr);
	DPRINTF(1, ("crypto_cert: %s\n", statstr));
	return (ret);
}