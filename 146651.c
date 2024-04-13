int dane_raw_tlsa(dane_state_t s, dane_query_t *r, char *const*dane_data, const int *dane_data_len, int secure, int bogus)
{
	int ret = DANE_E_SUCCESS;
	unsigned int i;

	*r = calloc(1, sizeof(struct dane_query_st));
	if (*r == NULL)
		return gnutls_assert_val(DANE_E_MEMORY_ERROR);

	(*r)->data_entries = 0;

	for (i=0;i<MAX_DATA_ENTRIES;i++)
	  {
	  	if (dane_data[i] == NULL)
	  		break;

		if (dane_data_len[i] <= 3)
			return gnutls_assert_val(DANE_E_RECEIVED_CORRUPT_DATA);  

		(*r)->usage[i] = dane_data[i][0];
		(*r)->type[i] = dane_data[i][1];
		(*r)->match[i] = dane_data[i][2];
		(*r)->data[i].data = (void*)&dane_data[i][3];
		(*r)->data[i].size = dane_data_len[i] - 3;
		(*r)->data_entries++;
	  }

	if (!(s->flags & DANE_F_INSECURE) && !secure) {
		if (bogus)
			ret = gnutls_assert_val(DANE_E_INVALID_DNSSEC_SIG);
		else
			ret = gnutls_assert_val(DANE_E_NO_DNSSEC_SIG);
	}

	/* show security status */
	if (secure) {
		(*r)->status = DANE_QUERY_DNSSEC_VERIFIED;
	} else if (bogus) {
	        gnutls_assert();
		(*r)->status = DANE_QUERY_BOGUS;
	} else {
	        gnutls_assert();
	        (*r)->status = DANE_QUERY_NO_DNSSEC;
        }

	return ret;
}