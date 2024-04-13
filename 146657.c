int dane_query_tlsa(dane_state_t s, dane_query_t *r, const char* host, const char* proto, unsigned int port)
{
	char ns[1024];
	int ret;
        struct ub_result *result;

	snprintf(ns, sizeof(ns), "_%u._%s.%s", port, proto, host);

	/* query for webserver */
	ret = ub_resolve(s->ctx, ns, 52, 1, &result);
	if(ret != 0) {
		return gnutls_assert_val(DANE_E_RESOLVING_ERROR);
	}

/* show first result */
	if(!result->havedata) {
                ub_resolve_free (result);
		return gnutls_assert_val(DANE_E_NO_DANE_DATA);
	}

        ret = dane_raw_tlsa (s, r, result->data, result->len, result->secure, result->bogus);
        if (*r == NULL) {
                ub_resolve_free (result);
		return ret;
        }
        (*r)->result = result;
	return ret;
}