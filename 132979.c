cert_parse(
	const u_char *asn1cert,	/* X509 certificate */
	long	len,		/* certificate length */
	tstamp_t fstamp		/* filestamp */
	)
{
	X509	*cert;		/* X509 certificate */
	X509_EXTENSION *ext;	/* X509v3 extension */
	struct cert_info *ret;	/* certificate info/value */
	BIO	*bp;
	char	pathbuf[MAXFILENAME];
	const u_char *ptr;
	char	*pch;
	int	temp, cnt, i;
	struct calendar fscal;

	/*
	 * Decode ASN.1 objects and construct certificate structure.
	 */
	ptr = asn1cert;
	if ((cert = d2i_X509(NULL, &ptr, len)) == NULL) {
		msyslog(LOG_ERR, "cert_parse: %s",
		    ERR_error_string(ERR_get_error(), NULL));
		return (NULL);
	}
#ifdef DEBUG
	if (debug > 1)
		X509_print_fp(stdout, cert);
#endif

	/*
	 * Extract version, subject name and public key.
	 */
	ret = emalloc_zero(sizeof(*ret));
	if ((ret->pkey = X509_get_pubkey(cert)) == NULL) {
		msyslog(LOG_ERR, "cert_parse: %s",
		    ERR_error_string(ERR_get_error(), NULL));
		cert_free(ret);
		X509_free(cert);
		return (NULL);
	}
	ret->version = X509_get_version(cert);
	X509_NAME_oneline(X509_get_subject_name(cert), pathbuf,
	    sizeof(pathbuf));
	pch = strstr(pathbuf, "CN=");
	if (NULL == pch) {
		msyslog(LOG_NOTICE, "cert_parse: invalid subject %s",
		    pathbuf);
		cert_free(ret);
		X509_free(cert);
		return (NULL);
	}
	ret->subject = estrdup(pch + 3);

	/*
	 * Extract remaining objects. Note that the NTP serial number is
	 * the NTP seconds at the time of signing, but this might not be
	 * the case for other authority. We don't bother to check the
	 * objects at this time, since the real crunch can happen only
	 * when the time is valid but not yet certificated.
	 */
	ret->nid = OBJ_obj2nid(cert->cert_info->signature->algorithm);
	ret->digest = (const EVP_MD *)EVP_get_digestbynid(ret->nid);
	ret->serial =
	    (u_long)ASN1_INTEGER_get(X509_get_serialNumber(cert));
	X509_NAME_oneline(X509_get_issuer_name(cert), pathbuf,
	    sizeof(pathbuf));
	if ((pch = strstr(pathbuf, "CN=")) == NULL) {
		msyslog(LOG_NOTICE, "cert_parse: invalid issuer %s",
		    pathbuf);
		cert_free(ret);
		X509_free(cert);
		return (NULL);
	}
	ret->issuer = estrdup(pch + 3);
	asn_to_calendar(X509_get_notBefore(cert), &(ret->first));
	asn_to_calendar(X509_get_notAfter(cert), &(ret->last));

	/*
	 * Extract extension fields. These are ad hoc ripoffs of
	 * currently assigned functions and will certainly be changed
	 * before prime time.
	 */
	cnt = X509_get_ext_count(cert);
	for (i = 0; i < cnt; i++) {
		ext = X509_get_ext(cert, i);
		temp = OBJ_obj2nid(ext->object);
		switch (temp) {

		/*
		 * If a key_usage field is present, we decode whether
		 * this is a trusted or private certificate. This is
		 * dorky; all we want is to compare NIDs, but OpenSSL
		 * insists on BIO text strings.
		 */
		case NID_ext_key_usage:
			bp = BIO_new(BIO_s_mem());
			X509V3_EXT_print(bp, ext, 0, 0);
			BIO_gets(bp, pathbuf, sizeof(pathbuf));
			BIO_free(bp);
			if (strcmp(pathbuf, "Trust Root") == 0)
				ret->flags |= CERT_TRUST;
			else if (strcmp(pathbuf, "Private") == 0)
				ret->flags |= CERT_PRIV;
			DPRINTF(1, ("cert_parse: %s: %s\n",
				    OBJ_nid2ln(temp), pathbuf));
			break;

		/*
		 * If a NID_subject_key_identifier field is present, it
		 * contains the GQ public key.
		 */
		case NID_subject_key_identifier:
			ret->grpkey = BN_bin2bn(&ext->value->data[2],
			    ext->value->length - 2, NULL);
			/* fall through */
		default:
			DPRINTF(1, ("cert_parse: %s\n",
				    OBJ_nid2ln(temp)));
			break;
		}
	}
	if (strcmp(ret->subject, ret->issuer) == 0) {

		/*
		 * If certificate is self signed, verify signature.
		 */
		if (X509_verify(cert, ret->pkey) <= 0) {
			msyslog(LOG_NOTICE,
			    "cert_parse: signature not verified %s",
			    ret->subject);
			cert_free(ret);
			X509_free(cert);
			return (NULL);
		}
	} else {

		/*
		 * Check for a certificate loop.
		 */
		if (strcmp((const char *)hostval.ptr, ret->issuer) == 0) {
			msyslog(LOG_NOTICE,
			    "cert_parse: certificate trail loop %s",
			    ret->subject);
			cert_free(ret);
			X509_free(cert);
			return (NULL);
		}
	}

	/*
	 * Verify certificate valid times. Note that certificates cannot
	 * be retroactive.
	 */
	(void)ntpcal_ntp_to_date(&fscal, fstamp, NULL);
	if ((calcomp(&(ret->first), &(ret->last)) > 0)
	|| (calcomp(&(ret->first), &fscal) < 0)) {
		msyslog(LOG_NOTICE,
		    "cert_parse: invalid times %s first %u-%02u-%02uT%02u:%02u:%02u last %u-%02u-%02uT%02u:%02u:%02u fstamp %u-%02u-%02uT%02u:%02u:%02u",
		    ret->subject,
		    ret->first.year, ret->first.month, ret->first.monthday,
		    ret->first.hour, ret->first.minute, ret->first.second,
		    ret->last.year, ret->last.month, ret->last.monthday,
		    ret->last.hour, ret->last.minute, ret->last.second,
		    fscal.year, fscal.month, fscal.monthday,
		    fscal.hour, fscal.minute, fscal.second);
		cert_free(ret);
		X509_free(cert);
		return (NULL);
	}

	/*
	 * Build the value structure to sign and send later.
	 */
	ret->cert.fstamp = htonl(fstamp);
	ret->cert.vallen = htonl(len);
	ret->cert.ptr = emalloc(len);
	memcpy(ret->cert.ptr, asn1cert, len);
	X509_free(cert);
	return (ret);
}