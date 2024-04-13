cert_free(
	struct cert_info *cinf	/* certificate info/value structure */ 
	)
{
	if (cinf->pkey != NULL)
		EVP_PKEY_free(cinf->pkey);
	if (cinf->subject != NULL)
		free(cinf->subject);
	if (cinf->issuer != NULL)
		free(cinf->issuer);
	if (cinf->grpkey != NULL)
		BN_free(cinf->grpkey);
	value_free(&cinf->cert);
	free(cinf);
}