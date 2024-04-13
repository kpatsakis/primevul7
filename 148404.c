rrinternal_get_ttl(sldns_buffer* strbuf, char* token, size_t token_len,
	int* not_there, uint32_t* ttl, uint32_t default_ttl)
{
	const char* endptr;
	if(sldns_bget_token(strbuf, token, "\t\n ", token_len) == -1) {
		return RET_ERR(LDNS_WIREPARSE_ERR_SYNTAX_TTL,
			sldns_buffer_position(strbuf));
	}
	*ttl = (uint32_t) sldns_str2period(token, &endptr);

	if (strlen(token) > 0 && !isdigit((unsigned char)token[0])) {
		*not_there = 1;
		/* ah, it's not there or something */
		if (default_ttl == 0) {
			*ttl = LDNS_DEFAULT_TTL;
		} else {
			*ttl = default_ttl;
		}
	}
	return LDNS_WIREPARSE_ERR_OK;
}