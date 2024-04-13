crypto_setup(void)
{
	struct pkey_info *pinfo; /* private/public key */
	char	filename[MAXFILENAME]; /* file name buffer */
	char	hostname[MAXFILENAME]; /* host name buffer */
	char	*randfile;
	char	statstr[NTP_MAXSTRLEN]; /* statistics for filegen */
	l_fp	seed;		/* crypto PRNG seed as NTP timestamp */
	u_int	len;
	int	bytes;
	u_char	*ptr;

	/*
	 * Check for correct OpenSSL version and avoid initialization in
	 * the case of multiple crypto commands.
	 */
	if (crypto_flags & CRYPTO_FLAG_ENAB) {
		msyslog(LOG_NOTICE,
		    "crypto_setup: spurious crypto command");
		return;
	}
	ssl_check_version();

	/*
	 * Load required random seed file and seed the random number
	 * generator. Be default, it is found as .rnd in the user home
	 * directory. The root home directory may be / or /root,
	 * depending on the system. Wiggle the contents a bit and write
	 * it back so the sequence does not repeat when we next restart.
	 */
	if (!RAND_status()) {
		if (rand_file == NULL) {
			RAND_file_name(filename, sizeof(filename));
			randfile = filename;
		} else if (*rand_file != '/') {
			snprintf(filename, sizeof(filename), "%s/%s",
			    keysdir, rand_file);
			randfile = filename;
		} else
			randfile = rand_file;

		if ((bytes = RAND_load_file(randfile, -1)) == 0) {
			msyslog(LOG_ERR,
			    "crypto_setup: random seed file %s missing",
			    randfile);
			exit (-1);
		}
		get_systime(&seed);
		RAND_seed(&seed, sizeof(l_fp));
		RAND_write_file(randfile);
		DPRINTF(1, ("crypto_setup: OpenSSL version %lx random seed file %s bytes read %d\n",
			    SSLeay(), randfile, bytes));
	}

	/*
	 * Initialize structures.
	 */
	gethostname(hostname, sizeof(hostname));
	if (host_filename != NULL)
		strlcpy(hostname, host_filename, sizeof(hostname));
	if (passwd == NULL)
		passwd = estrdup(hostname);
	memset(&hostval, 0, sizeof(hostval));
	memset(&pubkey, 0, sizeof(pubkey));
	memset(&tai_leap, 0, sizeof(tai_leap));

	/*
	 * Load required host key from file "ntpkey_host_<hostname>". If
	 * no host key file is not found or has invalid password, life
	 * as we know it ends. The host key also becomes the default
	 * sign key. 
	 */
	snprintf(filename, sizeof(filename), "ntpkey_host_%s", hostname);
	pinfo = crypto_key(filename, passwd, NULL);
	if (pinfo == NULL) {
		msyslog(LOG_ERR,
		    "crypto_setup: host key file %s not found or corrupt",
		    filename);
		exit (-1);
	}
	if (pinfo->pkey->type != EVP_PKEY_RSA) {
		msyslog(LOG_ERR,
		    "crypto_setup: host key is not RSA key type");
		exit (-1);
	}
	host_pkey = pinfo->pkey;
	sign_pkey = host_pkey;
	hostval.fstamp = htonl(pinfo->fstamp);
	
	/*
	 * Construct public key extension field for agreement scheme.
	 */
	len = i2d_PublicKey(host_pkey, NULL);
	ptr = emalloc(len);
	pubkey.ptr = ptr;
	i2d_PublicKey(host_pkey, &ptr);
	pubkey.fstamp = hostval.fstamp;
	pubkey.vallen = htonl(len);

	/*
	 * Load optional sign key from file "ntpkey_sign_<hostname>". If
	 * available, it becomes the sign key.
	 */
	snprintf(filename, sizeof(filename), "ntpkey_sign_%s", hostname);
	pinfo = crypto_key(filename, passwd, NULL);
	if (pinfo != NULL)
		sign_pkey = pinfo->pkey;

	/*
	 * Load required certificate from file "ntpkey_cert_<hostname>".
	 */
	snprintf(filename, sizeof(filename), "ntpkey_cert_%s", hostname);
	cinfo = crypto_cert(filename);
	if (cinfo == NULL) {
		msyslog(LOG_ERR,
		    "crypto_setup: certificate file %s not found or corrupt",
		    filename);
		exit (-1);
	}
	cert_host = cinfo;
	sign_digest = cinfo->digest;
	sign_siglen = EVP_PKEY_size(sign_pkey);
	if (cinfo->flags & CERT_PRIV)
		crypto_flags |= CRYPTO_FLAG_PRIV;

	/*
	 * The certificate must be self-signed.
	 */
	if (strcmp(cinfo->subject, cinfo->issuer) != 0) {
		msyslog(LOG_ERR,
		    "crypto_setup: certificate %s is not self-signed",
		    filename);
		exit (-1);
	}
	hostval.ptr = estrdup(cinfo->subject);
	hostval.vallen = htonl(strlen(cinfo->subject));
	sys_hostname = hostval.ptr;
	ptr = (u_char *)strchr(sys_hostname, '@');
	if (ptr != NULL)
		sys_groupname = estrdup((char *)++ptr);
	if (ident_filename != NULL)
		strlcpy(hostname, ident_filename, sizeof(hostname));

	/*
	 * Load optional IFF parameters from file
	 * "ntpkey_iffkey_<hostname>".
	 */
	snprintf(filename, sizeof(filename), "ntpkey_iffkey_%s",
	    hostname);
	iffkey_info = crypto_key(filename, passwd, NULL);
	if (iffkey_info != NULL)
		crypto_flags |= CRYPTO_FLAG_IFF;

	/*
	 * Load optional GQ parameters from file
	 * "ntpkey_gqkey_<hostname>".
	 */
	snprintf(filename, sizeof(filename), "ntpkey_gqkey_%s",
	    hostname);
	gqkey_info = crypto_key(filename, passwd, NULL);
	if (gqkey_info != NULL)
		crypto_flags |= CRYPTO_FLAG_GQ;

	/*
	 * Load optional MV parameters from file
	 * "ntpkey_mvkey_<hostname>".
	 */
	snprintf(filename, sizeof(filename), "ntpkey_mvkey_%s",
	    hostname);
	mvkey_info = crypto_key(filename, passwd, NULL);
	if (mvkey_info != NULL)
		crypto_flags |= CRYPTO_FLAG_MV;

	/*
	 * We met the enemy and he is us. Now strike up the dance.
	 */
	crypto_flags |= CRYPTO_FLAG_ENAB | (cinfo->nid << 16);
	snprintf(statstr, sizeof(statstr), "setup 0x%x host %s %s",
	    crypto_flags, hostname, OBJ_nid2ln(cinfo->nid));
	record_crypto_stats(NULL, statstr);
	DPRINTF(1, ("crypto_setup: %s\n", statstr));
}