rrinternal_get_class(sldns_buffer* strbuf, char* token, size_t token_len,
	int* not_there, uint16_t* cl)
{
	/* if 'not_there' then we got token from previous parse routine */
	if(!*not_there) {
		/* parse new token for class */
		if(sldns_bget_token(strbuf, token, "\t\n ", token_len) == -1) {
			return RET_ERR(LDNS_WIREPARSE_ERR_SYNTAX_CLASS,
				sldns_buffer_position(strbuf));
		}
	} else *not_there = 0;
	*cl = sldns_get_rr_class_by_name(token);
	/* class can be left out too, assume IN, current token must be type */
	if(*cl == 0 && strcmp(token, "CLASS0") != 0) {
		*not_there = 1;
		*cl = LDNS_RR_CLASS_IN;
	}
	return LDNS_WIREPARSE_ERR_OK;
}